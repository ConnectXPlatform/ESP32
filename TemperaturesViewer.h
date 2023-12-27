#pragma once
#include <map>
#include "src/input/sensors/TemperatureSensor.h"
#include "src/display/Screens.h"

class TemperaturesScreen : public Screen
{
public:
    TemperaturesScreen(DisplayHandler &displayHandler, uint8_t numColumns = 2);
    void showTemperatures(const std::vector<TemperatureReading> &readings);

protected:
    void prepareImpl() override {}
    void disposeImpl() override {}

private:
    const uint8_t numColumns;
};

class AddressMappingTemperatureSensor : public TemperatureSensorDecorator
{
public:

    AddressMappingTemperatureSensor(ITemperatureSensor& tempSensor, std::map<String, String> &addressToLabel);

    std::vector<TemperatureReading> getReadings() override;

private:
    std::map<String, String> &addressToLabel;
};
