#pragma once
#include "TemperatureSensor.h"


class InternalTemperatureSensor : public ITemperatureSensor
{
public:
    void init() override;

    const bool refreshData(bool block) override;

    const bool isRefreshCompleted() override;

    void getReadings(std::vector<TemperatureReading> &readings) override;
};

