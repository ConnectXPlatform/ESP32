#include "TemperaturesViewer.h"

#define TEXT_LEFT_PADDING 5
#define TEXT_TOP_PADDING 10
#define TEXT_COLOR 255, 255, 255

TemperaturesScreen::TemperaturesScreen(DisplayHandler &displayHandler, uint8_t numColumns)
    : Screen(displayHandler), numColumns(numColumns)
{
}

void TemperaturesScreen::showTemperatures(const std::vector<TemperatureReading> &readings)
{
    // Calculate the number of rows based on the number of columns
    const uint8_t numRows = (readings.size() + numColumns - 1) / numColumns;
    char textBuffer[51];
    bool stopLoop = false;

    int16_t displayWidth, displayHeight;
    displayHandler.getDimensions(&displayWidth, &displayHeight);
    int16_t columnWidth = displayWidth / numColumns;

    // Iterate through the readings
    for (uint8_t row = 0; row < numRows && !stopLoop; ++row)
    {
        for (uint8_t col = 0; col < numColumns; ++col)
        {
            // Calculate the index in the readings vector
            uint8_t index = row * numColumns + col;

            // Check if the index is within the size of the readings vector
            if (index >= readings.size())
            {
                stopLoop = true;
                break;
            }
            const TemperatureReading &reading = readings[index];

            // Calculate the coordinates for the label
            int16_t x = col * columnWidth + TEXT_LEFT_PADDING;
            int16_t y = row * 30 + TEXT_TOP_PADDING;

            snprintf(textBuffer, sizeof(textBuffer), "%s: %.2f°%c", reading.sensorAddress, reading.temperatureValue, reading.unit == Celsius ? 'C' : 'F');
            textBuffer[sizeof(textBuffer) - 1] = '\0';
            // Display the temperature reading
            displayHandler.showLabel(textBuffer, MediumText, x, y, TEXT_COLOR);
        }
    }
}

AddressMappingTemperatureSensor::AddressMappingTemperatureSensor(ITemperatureSensor &tempSensor)
    : TemperatureSensorDecorator(tempSensor)
{
}

std::vector<TemperatureReading> AddressMappingTemperatureSensor::getReadings()
{
    std::vector<TemperatureReading> readings = decorated.getReadings();

    for (size_t i = 0; i < readings.size(); i++)
    {
        TemperatureReading &reading = readings[i];
        // Check if the sensor address has a corresponding label in the map.
        auto it = addressToLabel.find(reading.sensorAddress);
        if (it != addressToLabel.end())
        {
            // Use the mapped label if available.
            reading.sensorAddress = it->second;
        }
    }

    return readings;
}

void AddressMappingTemperatureSensor::setAddressToLabelMap(std::map<String, String> &map)
{
    addressToLabel = map;
}
