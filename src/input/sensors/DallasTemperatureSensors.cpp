#include <OneWire.h>
#include "DallasTemperatureSensors.h"

void deviceAddressToString(const uint8_t *address, char output[17])
{
    for (uint8_t i = 0, j = 0; i < 8; i++, j += 2)
    {
        snprintf(output + j, 3, "%02X", address[i]);
    }
}

DallasTemperatureSensors::DallasTemperatureSensors(uint8_t sensorsPin)
{
    OneWire *wire = new OneWire(sensorsPin);
    sensors = DallasTemperature(wire);
}

void DallasTemperatureSensors::init()
{
    sensors.begin();
    deviceCount = sensors.getDeviceCount();
    cacheAddresses();
}

const bool DallasTemperatureSensors::refreshData(bool block)
{
    bool wasBlocked = sensors.getWaitForConversion();
    if (block)
        sensors.setWaitForConversion(true);
    sensors.requestTemperatures();
    if (block)
        sensors.setWaitForConversion(wasBlocked);
    return isRefreshCompleted();
}

const bool DallasTemperatureSensors::isRefreshCompleted()
{
    return sensors.isConversionComplete();
}

void DallasTemperatureSensors::getReadings(std::vector<TemperatureReading> &readings)
{
    readings.reserve(deviceCount);
    for (int i = 0; i < deviceCount; i++)
    {
        uint8_t *address;
        if (addresses == nullptr)
            sensors.getAddress(address, i);
        else
            address = addresses[i];

        float value = sensors.getTempC(address);
        if (value != DEVICE_DISCONNECTED_C)
        {
            TemperatureReading reading;
            reading.unit = Unit::Celsius;
            reading.temperatureValue = value;
            deviceAddressToString(address, reading.sensorAddress);
            readings.push_back(reading);
        }
    }
    readings.shrink_to_fit();
}

void DallasTemperatureSensors::cacheAddresses()
{
    if (addresses != nullptr)
    {
        delete[] addresses;
    }
    addresses = new DeviceAddress[deviceCount];
    for (uint8_t i = 0; i < deviceCount; i++)
    {
        sensors.getAddress(addresses[i], i);
    }
}
