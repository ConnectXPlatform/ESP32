#include <Arduino.h>
#include <string.h>
#include "MqttHandler.h"
#include "FirebaseAuth.h"
#include "../utils/StringUtils.h"

const char *connectionData PROGMEM = "{}";

char *stateToMessage(int state)
{
    // Source: https://pubsubclient.knolleary.net/api#state
    switch (state)
    {
    case MQTT_CONNECTION_TIMEOUT:
        return "The server didn't respond within the keepalive time";
    case MQTT_CONNECTION_LOST:
        return "The network connection was broken";
    case MQTT_CONNECT_FAILED:
        return "The network connection failed";
    case MQTT_DISCONNECTED:
        return "The client is disconnected cleanly";
    case MQTT_CONNECTED:
        return "The client is connected";
    case MQTT_CONNECT_BAD_PROTOCOL:
        return "The server doesn't support the requested version of MQTT";
    case MQTT_CONNECT_BAD_CLIENT_ID:
        return "The server rejected the client identifier";
    case MQTT_CONNECT_UNAVAILABLE:
        return "The server was unable to accept the connection";
    case MQTT_CONNECT_BAD_CREDENTIALS:
        return "The username/password were rejected";
    case MQTT_CONNECT_UNAUTHORIZED:
        return "The client was not authorized to connect";
    default:
        return "Unknown error";
    }
}

void connectToMqtt(PubSubClient &mqttClient, const char *clientId, const char *deviceId, uint8_t maxRetries)
{
    uint8_t retriesLeft = maxRetries;
    mqttClient.setBufferSize(2048);
    do
    {
        Serial.println(F("Attempting MQTT connection..."));
        // Attempt to connect
        if (!mqttClient.connect(clientId, deviceId, Auth.getAuthToken()))
        {
            Serial.print(F("failed, error="));
            Serial.println(stateToMessage(mqttClient.state()));
            Serial.println(F("Retrying in 5 seconds"));
            // Wait 5 seconds before retrying
            delay(5000);
        }
        retriesLeft--;
        Serial.printf("Retries left: %d\n", retriesLeft);
    } while (!mqttClient.connected() && retriesLeft > 0);
    mqttClient.setBufferSize(1024);
}

MQTTHandler::MQTTHandler(const char *deviceId, Client &client, uint8_t maxRetries)
    : mqttClient(client), maxRetries(maxRetries), deviceId(), topicCallbacks()
{
    strncpy(const_cast<char *>(this->deviceId), deviceId, DEVICE_ID_LENGTH + 1);
    // Register the callback
    mqttClient.setCallback([this](char *topic, uint8_t *payload, unsigned int payloadLength)
                           { this->callback(topic, payload, payloadLength); });
}

void MQTTHandler::connect(const char *server, const uint16_t port)
{
    if (mqttClient.connected())
        return;

    mqttClient.setServer(server, port);
    // Client id format is {user_id}_{device_id}
    char clientId[USER_ID_LENGTH + DEVICE_ID_LENGTH + 2] = {0};
    snprintf(clientId, USER_ID_LENGTH + DEVICE_ID_LENGTH + 2, "%s_%s", Auth.getUserId(), deviceId);
    connectToMqtt(mqttClient, clientId, deviceId, maxRetries);
    errorMessage = stateToMessage(mqttClient.state());
    if (isConnected())
        broadcast("Connected", connectionData, true);
    else
        ESP.restart();
}

void MQTTHandler::broadcast(const char *topic, const char *payload, const bool retain)
{
    // Full topic: <user_id>/ALL/<sender_device_id>/<payload_format>/<topic>
    // Full topic length: USER_ID_LENGTH + strlen("/ALL/") + DEVICE_ID_LENGTH + strlen("/json/") + MAX_TOPIC_LENGTH + 1 (for '\0')
    char buffer[USER_ID_LENGTH + 4 + DEVICE_ID_LENGTH + 6 + MAX_TOPIC_LENGTH + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/ALL/%s/json/%s", Auth.getUserId(), deviceId, topic);
    mqttClient.publish(buffer, payload, retain);
}

void MQTTHandler::respond(const MQTTMessage &message, const char *payload, const ResponseStatus status)
{
    // Full topic: <user_id>/<device_id>/<message_id>/<payload_format>/<response_status>/<topic>
    // Full topic length: USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen(messageId) + strlen("/json/") + MAX_TOPIC_LENGTH + RESPONSE_STATUS_LENGTH + 3 (for every '/') + 1 (for '\0')
    char buffer[USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen(message.messageId) + 6 + MAX_TOPIC_LENGTH + RESPONSE_STATUS_LENGTH + 3 + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/%s/%s/json/%02x/%s", Auth.getUserId(), deviceId, message.messageId, status, message.topic);
    if (status != ResponseStatus::InProgress)
    {
        disposeMessage(message);
    }
    boolean result = mqttClient.publish(buffer, payload);
}

void MQTTHandler::subscribe(const char *topic, CallbackFunction callback)
{
    // Full topic: <user_id>/<device_id>/<message_id>/<payload_format>/<topic>
    // Topic with wildcards: <user_id>/<device_id>/+/+/<topic>
    // Topic with wildcards length: USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen("/+/+/") + MAX_TOPIC_LENGTH + 1 (for every '/') + 1 (for '\0')
    char buffer[USER_ID_LENGTH + DEVICE_ID_LENGTH + 5 + MAX_TOPIC_LENGTH + 1 + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/%s/+/+/%s", Auth.getUserId(), deviceId, topic);

    mqttClient.subscribe(buffer);

    topicCallbacks[topic] = callback;
}

void MQTTHandler::update()
{
    if (!mqttClient.loop())
    {
        Serial.println(stateToMessage(mqttClient.state()));
        reconnect();
    }
}

void MQTTHandler::reconnect()
{
    // Client id format is {user_id}_{device_id}
    char clientId[USER_ID_LENGTH + DEVICE_ID_LENGTH + 2] = {0};
    snprintf(clientId, USER_ID_LENGTH + DEVICE_ID_LENGTH + 2, "%s_%s", Auth.getUserId(), deviceId);
    Serial.println(F("Reconnecting to MQTT broker..."));
    connectToMqtt(mqttClient, clientId, deviceId, maxRetries);
    errorMessage = stateToMessage(mqttClient.state());
    if (!isConnected())
        ESP.restart();
    broadcast("Connected", connectionData, true);
    // Resubscribe to all the topics
    char buffer[USER_ID_LENGTH + DEVICE_ID_LENGTH + 5 + MAX_TOPIC_LENGTH + 1 + 1] = {0};
    for (const auto topicCallbackPair : topicCallbacks)
    {
        snprintf(buffer, sizeof(buffer), "%s/%s/+/+/%s", Auth.getUserId(), deviceId, topicCallbackPair.first);
        mqttClient.subscribe(buffer);
    }
}

bool MQTTHandler::isConnected()
{
    return mqttClient.connected();
}

const char *MQTTHandler::getErrorMessage()
{
    return errorMessage;
}

void MQTTHandler::disposeMessage(const MQTTMessage &message)
{
    delete[] message.messageId;
    delete[] message.topic;
}

void MQTTHandler::callback(char *fullTopic, uint8_t *payload, unsigned int payloadLength)
{
    const String payloadAsJson = String(payload, payloadLength);
    // The full topic format: "<user_id>/<device_id>/<message_id>/<format>/<topic>"
    const char *topicSegments[5];
    uint segmentsLengths[5];
    splitString(fullTopic, '/', topicSegments, segmentsLengths, 5);

    char *topic = new char[segmentsLengths[4] + 1];
    strncpy(topic, topicSegments[4], segmentsLengths[4]);
    topic[segmentsLengths[4]] = 0;
    auto it = topicCallbacks.find(topic);
    if (it == topicCallbacks.end())
    {
        Serial.printf(PSTR("Unknown topic: %s\n"), topic);
        return;
    }
    char *messageId = new char[segmentsLengths[2] + 1];
    strncpy(messageId, topicSegments[2], segmentsLengths[2]);
    messageId[segmentsLengths[2]] = 0;
    it->second(MQTTMessage{messageId, deviceId, topic, payloadAsJson});
}
