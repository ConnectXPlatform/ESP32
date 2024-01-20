#pragma once
#include <map>
#include "src/input/sensors/TemperatureSensor.h"
#include "src/utils/TasksQueue.h"
#include "src/display/Screens.h"
#include "MqttPayloadConversion.h"

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
    AddressMappingTemperatureSensor(ITemperatureSensor &tempSensor);

    void getReadings(std::vector<TemperatureReading> &readings) override;

    void setAddressToLabelMap(std::map<String, String> &map);

private:
    std::map<String, String> addressToLabel;
};

class TemperatureRequestTask : public Task
{
public:
    TemperatureRequestTask(ITemperatureSensor &tempSensor);

    void start() override;
    bool update() override;

private:
    ITemperatureSensor &temperatureSensor;
};

class SendTempsDataTask : public Task
{
public:
    SendTempsDataTask(ITemperatureSensor &temperatureSensor, const TempsRequestPayload &requestSettings, std::function<void(String, bool isLastChunk)> sendFunction);

    void start() override;
    bool update() override;
private:
    ITemperatureSensor &temperatureSensor;
    std::vector<TemperatureReading> data;
    std::function<void(String, bool isLastChunk)> sendFunction;
    const uint chunkSize;
    size_t numChunks;
    size_t chunkIndex;
    DynamicJsonDocument jsonDocument;
    String serializedChunk;
};
