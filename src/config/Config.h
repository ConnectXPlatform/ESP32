#pragma once
#include <Stream.h>

/*
Configuration files:
MQTT.json:
{
    "userId": "",
    "deviceId": "",
    "serverURL": "",
    "port": 1883
}

WiFi.json:
{
    "ssid": "",
    "password": ""
}
*/
struct MqttSettings
{
    String userId;
    String deviceId;
    String serverURL;
    uint16_t port;
};
struct WifiSettings
{
    String ssid;
    String password;
};

MqttSettings &loadMqttSettings(Stream &jsonFile);
WifiSettings &loadWiFiSettings(Stream &jsonFile);
