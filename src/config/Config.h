#pragma once
#include <Arduino.h>
#include <Stream.h>
#include <map>

/*
Configuration files:
MQTT.json:
{
    "deviceId": "",
    "serverURL": "",
    "port": 1883
}

WiFi.json:
{
    "ssid": "",
    "password": ""
}

Firebase.json:
{
    "email": "",
    "password": "",
    "apiKey": ""
}
*/
struct MqttSettings
{
    String deviceId;
    String serverURL;
    uint16_t port;
};
struct WifiSettings
{
    String ssid;
    String password;
};
struct FirebaseSettings
{
    String email;
    String password;
    String apiKey;
};

MqttSettings loadMqttSettings(Stream &jsonFile);
WifiSettings loadWiFiSettings(Stream &jsonFile);
FirebaseSettings loadFirebaseSettingsSettings(Stream &jsonFile);
std::map<String, String> loadSensorsAdressesMap(Stream &jsonFile);

