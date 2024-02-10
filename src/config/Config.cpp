#include <ArduinoJson.h>
#include "Config.h"

#define DEFAULT_MQTT_PORT 1883

MqttSettings loadMqttSettings(Stream &jsonFile)
{
    // Size calculated using: https://arduinojson.org/v6/assistant (plus some buffer)
    StaticJsonDocument<100> jsonDoc;

    DeserializationError error = deserializeJson(jsonDoc, jsonFile);
    if (error)
    {
        throw std::runtime_error(error.c_str());
    }

    MqttSettings settings;

    settings.deviceId = jsonDoc["deviceId"].as<String>();
    settings.serverURL = jsonDoc["serverURL"].as<String>();
    uint16_t port;
    if (jsonDoc.containsKey("port"))
    {
        settings.port = jsonDoc["port"].as<uint16_t>();
    }
    else
    {
        settings.port = DEFAULT_MQTT_PORT;
    }

    return settings;
}

WifiSettings loadWiFiSettings(Stream &jsonFile)
{
    // Size calculated using: https://arduinojson.org/v6/assistant (plus some buffer)
    StaticJsonDocument<100> jsonDoc;

    DeserializationError error = deserializeJson(jsonDoc, jsonFile);
    if (error)
    {
        throw std::runtime_error(error.c_str());
    }

    WifiSettings settings;

    settings.ssid = jsonDoc["ssid"].as<String>();
    settings.password = jsonDoc["password"].as<String>();

    return settings;
}

FirebaseSettings loadFirebaseSettingsSettings(Stream &jsonFile)
{
    // Size calculated using: https://arduinojson.org/v6/assistant (plus some buffer)
    StaticJsonDocument<100> jsonDoc;

    DeserializationError error = deserializeJson(jsonDoc, jsonFile);
    if (error)
    {
        throw std::runtime_error(error.c_str());
    }

    FirebaseSettings settings;

    settings.email = jsonDoc["email"].as<String>();
    settings.password = jsonDoc["password"].as<String>();
    settings.apiKey = jsonDoc["apiKey"].as<String>();

    return settings;
}

std::map<String, String> loadSensorsAdressesMap(Stream &jsonFile)
{
    StaticJsonDocument<512> jsonDoc;

    DeserializationError error = deserializeJson(jsonDoc, jsonFile);
    if (error)
    {
        throw std::runtime_error(error.c_str());
    }

    std::map<String, String> map;
    for (auto entry : jsonDoc.as<JsonObject>())
    {
        map[entry.key().c_str()] = entry.value().as<String>();
    }
    return map;
}
