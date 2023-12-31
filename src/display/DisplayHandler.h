#pragma once
#include "Arduino.h"

enum TextSize
{
    SmallText = 1,
    MediumText = 2,
    LargeText = 3
};

class DisplayHandler
{
public:
    /// @brief Initializes the display and shows an empty screen.
    virtual void init() = 0;
    /// @brief Shows an empty screen.
    virtual void clearScreen() = 0;
    /// @brief Prints a label in the given position.
    /// @param label The text to show.
    /// @param textSize The size of the text to show.
    /// @param x The X coordinate of the label (the distance from the left edge of the display).
    /// @param y The Y coordinate of the label (the distance from the top edge of the display).
    /// @param color The text's color to use when printing the label on the screen.
    /// @return The number of lines this label used.
    virtual uint8_t showLabel(const char *text, TextSize TextSize, int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) = 0;
    /// @brief Displays large error message in the center of the screen.
    /// @param message The error message to display.
    virtual void showErrorMessage(const char *message) = 0;
    /// @brief Returns the width and height of the screen.
    /// @param width 
    /// @param height 
    virtual void getDimensions(int16_t* width, int16_t* height) = 0;
};
