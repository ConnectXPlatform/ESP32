#include "TemperatureSensor.h"

TemperatureSensorDecorator::TemperatureSensorDecorator(ITemperatureSensor &tempSensor)
    : decorated(tempSensor) {}
void TemperatureSensorDecorator::init() { decorated.init(); }
const bool TemperatureSensorDecorator::refreshData(bool block) { return decorated.refreshData(block); }
const bool TemperatureSensorDecorator::isRefreshCompleted() { return decorated.isRefreshCompleted(); }
void TemperatureSensorDecorator::getReadings(std::vector<TemperatureReading> &readings) { decorated.getReadings(readings); }

TimedCachingTemperatureSensor::TimedCachingTemperatureSensor(ITemperatureSensor &tempSensor, unsigned long readingsInterval)
    : TemperatureSensorDecorator(tempSensor), readingsInterval(readingsInterval)
{
}

const bool TimedCachingTemperatureSensor::refreshData(bool block)
{
    if (latestData.size() == 0 || millis() >= lastReadingTime + readingsInterval)
    {
        decorated.refreshData(block);
        lastReadingTime = millis();
        retrievingNewData = true;
    }
    return isRefreshCompleted();
}

const bool TimedCachingTemperatureSensor::isRefreshCompleted()
{
    return !retrievingNewData || decorated.isRefreshCompleted();
}

void TimedCachingTemperatureSensor::getReadings(std::vector<TemperatureReading> &readings)
{
    if (retrievingNewData)
    {
        // Clear the old cached data
        latestData.clear();
        decorated.getReadings(latestData);
        retrievingNewData = false;
    }
    readings = latestData;
}
