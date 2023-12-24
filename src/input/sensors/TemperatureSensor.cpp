#include "TemperatureSensor.h"

TimedCachingTemperatureSensor::TimedCachingTemperatureSensor(ITemperatureSensor &tempSensor, unsigned long readingsInterval)
    : decorated(tempSensor), readingsInterval(readingsInterval)
{
}

void TimedCachingTemperatureSensor::init()
{
    decorated.init();
}

const bool TimedCachingTemperatureSensor::refreshData(bool block)
{
    if (millis() >= lastReadingTime + readingsInterval)
    {
        decorated.refreshData(block);
        lastReadingTime = millis();
        issuedAReading = true;
    }
    return isRefreshCompleted();
}

const bool TimedCachingTemperatureSensor::isRefreshCompleted()
{
    return !issuedAReading || decorated.isRefreshCompleted();
}

std::vector<TemperatureReading> TimedCachingTemperatureSensor::getReadings()
{
    if (issuedAReading)
    {
        latestData = decorated.getReadings();
        issuedAReading = false;
    }
    return latestData;
}
