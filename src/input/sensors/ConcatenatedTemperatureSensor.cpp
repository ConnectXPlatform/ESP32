#include "ConcatenatedTemperatureSensor.h"

void ConcatenatedTemperatureSensor::init()
{
    for (ITemperatureSensor *sensor : sensors)
    {
        sensor->init();
    }
}

const bool ConcatenatedTemperatureSensor::refreshData(bool block)
{
    bool result = true;
    for (ITemperatureSensor *sensor : sensors)
    {
        result |= sensor->refreshData();
    }
    return result;
}

const bool ConcatenatedTemperatureSensor::isRefreshCompleted()
{
    for (ITemperatureSensor *sensor : sensors)
    {
        if (!sensor->isRefreshCompleted())
            return false;
    }
    return true;
}

void ConcatenatedTemperatureSensor::getReadings(std::vector<TemperatureReading> &readings)
{
    for (ITemperatureSensor *sensor : sensors)
    {
        sensor->getReadings(readings);
    }
}

void ConcatenatedTemperatureSensor::addSensor(ITemperatureSensor *sensor)
{
    sensors.push_back(sensor);
}
