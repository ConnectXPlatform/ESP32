#pragma once
#include "TemperatureSensor.h"


class ConcatenatedTemperatureSensor : public ITemperatureSensor
{
public:
    void init() override;

    const bool refreshData(bool block) override;

    const bool isRefreshCompleted() override;

    void getReadings(std::vector<TemperatureReading> &readings) override;

    void addSensor(ITemperatureSensor* sensor);
private:
    std::vector<ITemperatureSensor*> sensors;
};

