#pragma once
#include <DallasTemperature.h>
#include "TemperatureSensor.h"

class DallasTemperatureSensors : public ITemperatureSensor
{
public:
    DallasTemperatureSensors(uint8_t sensorsPin);

    void init() override;

    const bool refreshData(bool block) override;

    const bool isRefreshCompleted() override;

    virtual void getReadings(std::vector<TemperatureReading> &readings) override;

private:
    DallasTemperature sensors;
    uint8_t deviceCount;
    DeviceAddress *addresses;

    void cacheAddresses();
};
