#include "TemperatureSensor.h"

TemperatureSensorDecorator::TemperatureSensorDecorator(ITemperatureSensor &tempSensor)
    : decorated(tempSensor) {}
void TemperatureSensorDecorator::init() { decorated.init(); }
const bool TemperatureSensorDecorator::refreshData(bool block) { return decorated.refreshData(block); }
const bool TemperatureSensorDecorator::isRefreshCompleted() { return decorated.isRefreshCompleted(); }
std::vector<TemperatureReading> TemperatureSensorDecorator::getReadings() { return decorated.getReadings(); }

TimedCachingTemperatureSensor::TimedCachingTemperatureSensor(ITemperatureSensor &tempSensor, unsigned long readingsInterval)
    : TemperatureSensorDecorator(tempSensor), readingsInterval(readingsInterval)
{
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
