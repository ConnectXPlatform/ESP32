#include <Arduino.h>
#include <string.h>
#include "MqttHandler.h"
#include "../utils/StringUtils.h"

const char *connectionData = "{}";

char *stateToMessage(int state)
{
    // Source: https://pubsubclient.knolleary.net/api#state
    switch (state)
    {
    case -4:
        return "The server didn't respond within the keepalive time";
    case -3:
        return "The network connection was broken";
    case -2:
        return "The network connection failed";
    case -1:
        return "The client is disconnected cleanly";
    case 0:
        return "The client is connected";
    case 1:
        return "The server doesn't support the requested version of MQTT";
    case 2:
        return "The server rejected the client identifier";
    case 3:
        return "The server was unable to accept the connection";
    case 4:
        return "The username/password were rejected";
    case 5:
        return "The client was not authorized to connect";
    default:
        return "Unknown error";
    }
}

MQTTHandler::MQTTHandler(const char *username, const char *deviceId, Client &client, uint8_t maxRetries)
    : mqttClient(client), maxRetries(maxRetries), userid(), deviceId(), topicCallbacks()
{
    strncpy(const_cast<char *>(this->userid), username, USER_ID_LENGTH + 1);
    strncpy(const_cast<char *>(this->deviceId), deviceId, DEVICE_ID_LENGTH + 1);
    // Register the callback
    mqttClient.setCallback([this](char *topic, uint8_t *payload, unsigned int payloadLength)
                           { this->callback(topic, payload, payloadLength); });
}

void MQTTHandler::connect(const char *server, const uint16_t port)
{
    if (mqttClient.connected())
        return;

    Serial.println(F("Connecting MQTT"));
    mqttClient.setServer(server, port);
    // Client id format is {user_id}_{device_id}
    char clientId[USER_ID_LENGTH + DEVICE_ID_LENGTH + 2] = {0};
    snprintf(clientId, USER_ID_LENGTH + DEVICE_ID_LENGTH + 2, "%s_%s", userid, deviceId);
    Serial.print(F("Connecting as: "));
    Serial.println(clientId);

    uint8_t retriesLeft = maxRetries;
    do
    {
        Serial.println(F("Attempting MQTT connection..."));
        if (!mqttClient.connect(clientId, userid, deviceId))
        {
            Serial.print(F("failed, error="));
            Serial.println(stateToMessage(mqttClient.state()));
            Serial.println(F("Retrying in 5 seconds"));
            // Wait 5 seconds before retrying
            delay(5000);
        }
        retriesLeft--;
        Serial.printf("Retries left: %d\n", retriesLeft);
    } while (!isConnected() && retriesLeft > 0);
    errorMessage = stateToMessage(mqttClient.state());
    if (isConnected())
        broadcast("Connected", connectionData, true);
}

void MQTTHandler::broadcast(const char *topic, const char *payload, const bool retain)
{
    // Full topic: <user_id>/ALL/<sender_device_id>/<payload_format>/<topic>
    // Full topic length: USER_ID_LENGTH + strlen("/ALL/") + DEVICE_ID_LENGTH + strlen("/json/") + MAX_TOPIC_LENGTH + 1 (for '\0')
    char buffer[USER_ID_LENGTH + 4 + DEVICE_ID_LENGTH + 6 + MAX_TOPIC_LENGTH + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/ALL/%s/json/%s", userid, deviceId, topic);
    mqttClient.publish(buffer, payload, retain);
}

void MQTTHandler::respond(const MQTTMessage& message, const char *payload, const ResponseStatus status)
{
    // Full topic: <user_id>/<device_id>/<message_id>/<payload_format>/<topic>/<response_status>
    // Full topic length: USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen(messageId) + strlen("/json/") + MAX_TOPIC_LENGTH + RESPONSE_STATUS_LENGTH + 3 (for every '/') + 1 (for '\0')
    char buffer[USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen(message.messageId) + 6 + MAX_TOPIC_LENGTH + RESPONSE_STATUS_LENGTH + 3 + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/%s/%s/json/%s/%02x", userid, deviceId, message.messageId, message.topic, status);
    mqttClient.publish(buffer, payload);
}

void MQTTHandler::subscribe(const char *topic, CallbackFunction callback)
{
    // Full topic: <user_id>/<device_id>/<message_id>/<payload_format>/<topic>
    // Topic with wildcards: <user_id>/<device_id>/+/+/<topic>
    // Topic with wildcards length: USER_ID_LENGTH + DEVICE_ID_LENGTH + strlen("/+/+/") + MAX_TOPIC_LENGTH + 1 (for every '/') + 1 (for '\0')
    char buffer[USER_ID_LENGTH + DEVICE_ID_LENGTH + 5 + MAX_TOPIC_LENGTH + 1 + 1] = {0};
    snprintf(buffer, sizeof(buffer), "%s/%s/+/+/%s", userid, deviceId, topic);

    Serial.print(F("Subscribing to: "));
    Serial.println(buffer);

    mqttClient.subscribe(buffer);

    topicCallbacks[topic] = callback;
}

void MQTTHandler::update()
{
    // Serial.println(stateToMessage(mqttClient.state()));
    if (!mqttClient.loop())
        reconnect();
}

void MQTTHandler::reconnect()
{
    // Client id format is {user_id}_{device_id}
    char clientId[USER_ID_LENGTH + DEVICE_ID_LENGTH + 2] = {0};
    snprintf(clientId, USER_ID_LENGTH + DEVICE_ID_LENGTH + 2, "%s_%s", userid, deviceId);
    Serial.println(F("Reconnecting to MQTT broker..."));
    uint8_t retriesLeft = maxRetries;
    do
    {
        Serial.println(F("Attempting MQTT reconnection..."));
        // Attempt to connect
        if (!mqttClient.connect(clientId, userid, deviceId))
        {
            Serial.print(F("failed, error="));
            Serial.println(stateToMessage(mqttClient.state()));
            Serial.println(F("Retrying in 5 seconds"));
            // Wait 5 seconds before retrying
            delay(5000);
        }
        retriesLeft--;
        Serial.printf("Retries left: %d\n", retriesLeft);
    } while (!isConnected() && retriesLeft > 0);
    errorMessage = stateToMessage(mqttClient.state());
    if (isConnected())
        broadcast("Connected", connectionData, true);
}

bool MQTTHandler::isConnected()
{
    return mqttClient.connected();
}

const char *MQTTHandler::getErrorMessage()
{
    return errorMessage;
}

void MQTTHandler::callback(char *fullTopic, uint8_t *payload, unsigned int payloadLength)
{
    Serial.println("Got message!");
    const String payloadAsJson = String(payload, payloadLength);
    Serial.print(F("Received payload: "));
    Serial.println(payloadAsJson);
    // The full topic format: "<user_id>/<device_id>/<message_id>/<format>/<topic>"
    const char *topicSegments[5];
    uint segmentsLengths[5];
    splitString(fullTopic, '/', topicSegments, segmentsLengths, 5);

    Serial.printf("Topic length: %d\n", segmentsLengths[4]);
    char topic[segmentsLengths[4] + 1] = {0};
    strncpy(topic, topicSegments[4], segmentsLengths[4]);
    auto it = topicCallbacks.find(topic);
    if (it == topicCallbacks.end())
    {
        Serial.printf(PSTR("Unknown topic: %s\n"), topic);
        return;
    }
    Serial.printf("Message id length: %d\n", segmentsLengths[2]);
    char messageId[segmentsLengths[2] + 1] = {0};
    strncpy(messageId, topicSegments[2], segmentsLengths[2]);
    it->second(MQTTMessage{messageId, deviceId, topic, payloadAsJson});
}
