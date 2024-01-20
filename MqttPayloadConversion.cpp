#include "MqttPayloadConversion.h"

#define DEFAULT_CHUNK_SIZE 5

TempsRequestPayload jsonToTempRequest(JsonDocument &jsonDoc)
{
    TempsRequestPayload payload;
    if (jsonDoc.containsKey("chunkSize"))
        payload.chunkSize = jsonDoc["chunkSize"].as<uint8_t>();
    else
        payload.chunkSize = DEFAULT_CHUNK_SIZE;

    return payload;
}

ButtonStatePayload jsonToButtonState(JsonDocument &jsonDoc)
{
    ButtonStatePayload payload;
    payload.newState = jsonDoc["newState"].as<uint8_t>();

    return payload;
}
