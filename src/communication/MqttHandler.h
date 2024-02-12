#pragma once
#include <PubSubClient.h>
#include <Client.h>
#include <map>

#define USER_ID_LENGTH 28
#define DEVICE_ID_LENGTH 20
#define MAX_TOPIC_LENGTH 30
#define RESPONSE_STATUS_LENGTH 2

struct MQTTMessage
{
    const char *messageId;
    const char *deviceId;
    const char *topic;
    const String payload;
};

enum ResponseStatus
{
    Error = 1,
    InProgress = 2,
    Done = 3,
    Empty = 4,
};

class MQTTHandler
{
    // Define a type for callback functions
    using CallbackFunction = std::function<void(const MQTTMessage message)>;

public:
    MQTTHandler(const char *deviceId, Client &client, uint8_t maxRetries = 5);

    void connect(const char *server, const uint16_t port);

    void broadcast(const char *topic, const char *payload, const bool retain = false);

    void respond(const MQTTMessage &message, const char *payload, const ResponseStatus status);

    void subscribe(const char *topic, CallbackFunction callback);

    void update();

    void reconnect();

    bool isConnected();

    const char *getErrorMessage();

    void disposeMessage(const MQTTMessage &message);

private:
    struct cmp_str
    {
        bool operator()(char const *a, char const *b) const
        {
            return strcmp(a, b) < 0;
        }
    };

    PubSubClient mqttClient;
    const uint8_t maxRetries;
    const char deviceId[DEVICE_ID_LENGTH + 1]; // Add 1 for the null-terminating character.
    std::map<const char *, CallbackFunction, cmp_str> topicCallbacks;
    char *errorMessage;

    void callback(char *topic, uint8_t *payload, unsigned int payloadLength);
};
