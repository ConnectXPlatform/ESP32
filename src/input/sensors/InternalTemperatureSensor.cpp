#include <esp32-hal.h>
#include "InternalTemperatureSensor.h"

void InternalTemperatureSensor::init()
{
    // No specific initialization required for the internal temperature sensor
}

const bool InternalTemperatureSensor::refreshData(bool block)
{
    // For the internal temperature sensor, reading data is instantaneous
    return true;
}

const bool InternalTemperatureSensor::isRefreshCompleted()
{
    // The internal temperature sensor always provides data immediately
    return true;
}

void InternalTemperatureSensor::getReadings(std::vector<TemperatureReading> &readings)
{
    float temperature = temperatureRead();
    readings.emplace_back("Internal sensor", temperature, Unit::Celsius);
}
