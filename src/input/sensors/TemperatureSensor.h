#pragma once
#include <Arduino.h>
#include <vector>

enum Unit
{
    Celsius,
    Fahrenheit
};
struct TemperatureReading
{
    String sensorAddress;   // Address or identifier of the sensor.
    float temperatureValue; // Temperature value.
    Unit unit;              // The value's unit of measurement.
};

class ITemperatureSensor
{
public:
    virtual ~ITemperatureSensor() {}

    /// @brief Initializes the temperature sensor.
    virtual void init() = 0;

    /// @brief Requests data from the temperature sensor.
    /// @param block Whether or not the function will block the current thread while obtaining new data.
    ///     If set to false, the request will run in the background, and you should use `isRefreshCompleted` to check if the new data is available.
    /// @return True if the data is available immediately, false otherwise.
    virtual const bool refreshData(bool block = true) = 0;

    /// @brief Checks if the last data request has completed.
    /// @return True if the newly requested data is available, false otherwise.
    virtual const bool isRefreshCompleted() = 0;

    /// @brief Retrieves all temperature readings from this sensor.
    /// @return A vector of `TemperatureReading` objects, each representing a temperature reading from this sensor.
    virtual std::vector<TemperatureReading> getReadings() = 0;
};

class TemperatureSensorDecorator : public ITemperatureSensor
{
public:
    virtual void init() override;

    virtual const bool refreshData(bool block = true) override;

    virtual const bool isRefreshCompleted() override;

    virtual std::vector<TemperatureReading> getReadings() override;

protected:
    ITemperatureSensor &decorated;

    TemperatureSensorDecorator(ITemperatureSensor &tempSensor);
};

class TimedCachingTemperatureSensor : public TemperatureSensorDecorator
{
public:
    TimedCachingTemperatureSensor(ITemperatureSensor &tempSensor, unsigned long readingsInterval);

    const bool refreshData(bool block = true) override;

    const bool isRefreshCompleted() override;

    std::vector<TemperatureReading> getReadings() override;

private:
    unsigned long lastReadingTime;
    unsigned long readingsInterval;
    std::vector<TemperatureReading> latestData;
    bool issuedAReading;
};
