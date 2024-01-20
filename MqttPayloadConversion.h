#pragma once
#include <ArduinoJson.h>

#define TempsRequestPayloadSize 16
#define ButtonStatePayloadSize 16

struct TempsRequestPayload
{
    uint8_t chunkSize;
};

struct ButtonStatePayload
{
    uint8_t newState;
};

template <typename T>
bool payloadToObject(const String &jsonString, const uint requiredSize, std::function<T(JsonDocument &)> converter, T *output)
{
    // Parse the JSON string into a JsonDocument
    DynamicJsonDocument jsonDoc(requiredSize);

    // Based on the online ArduinoJson Assistant, it is more memory efficient to use `char*` than `String` and `const char*`.
    char jsonChar[jsonString.length() + 1]; // +1 for the null terminator
    jsonString.toCharArray(jsonChar, sizeof(jsonChar));

    DeserializationError error = deserializeJson(jsonDoc, jsonChar);

    if (error)
    {
        Serial.print(F("Error parsing JSON: "));
        Serial.println(error.c_str());
        return false;
    }

    *output = converter(jsonDoc);
    return true;
}

TempsRequestPayload jsonToTempRequest(JsonDocument &jsonDoc);

ButtonStatePayload jsonToButtonState(JsonDocument &jsonDoc);
