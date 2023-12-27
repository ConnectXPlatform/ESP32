#pragma once
#include <Adafruit_ILI9341.h>
#include "DisplayHandler.h"

const uint8_t BaseTextSize = 8;

class ILI9341DisplayHandler : public DisplayHandler
{
public:
    ILI9341DisplayHandler(uint8_t cs, uint8_t dc, uint8_t rst);

    void init() override;

    void clearScreen() override;

    uint8_t showLabel(const char *label, TextSize textSize, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) override;

    void showErrorMessage(const char *errorMessage) override;

    void getDimensions(int16_t* width, int16_t* height) override;

private:
    Adafruit_ILI9341 display;
};
