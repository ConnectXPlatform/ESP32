#include "ILI9341DisplayHandler.h"

ILI9341DisplayHandler::ILI9341DisplayHandler(uint8_t cs, uint8_t dc, uint8_t rst) : display(cs, dc, rst) {}

void ILI9341DisplayHandler::init()
{
    display.begin();
    display.setRotation(1);
    clearScreen();
}

void ILI9341DisplayHandler::clearScreen()
{
    display.fillScreen(ILI9341_BLACK);
}

uint8_t ILI9341DisplayHandler::showLabel(const char *label, TextSize textSize, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = display.color565(r, g, b);
    display.setCursor(x, y);
    display.setTextSize(textSize);
    display.setTextColor(color);
    display.print(label);
    int16_t x1, y1;
    uint16_t width, height;
    display.getTextBounds(label, x, y, &x1, &y1, &width, &height);
    return height / (BaseTextSize * textSize);
}

void ILI9341DisplayHandler::showErrorMessage(const char *errorMessage)
{
    clearScreen();
    // Make sure the error message is centered.
    int16_t x1, y1;
    uint16_t width, height;
    display.getTextBounds(errorMessage, 0, 0, &x1, &y1, &width, &height);
    showLabel(errorMessage, LargeText, x1, (display.height() - height) / 2 - height, 255, 0, 0);
}

void ILI9341DisplayHandler::getDimensions(int16_t *width, int16_t *height)
{
    *width = display.width();
    *height = display.height();
}
