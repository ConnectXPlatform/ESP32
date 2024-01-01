#include <Arduino.h>
#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(const uint8_t buttonPin, const unsigned long maxPressTime)
    : buttonPin(buttonPin), maxPressTime(maxPressTime), onPressCallback(nullptr), onReleaseCallback(nullptr), isPressed(false)
{
}

void ButtonHandler::init()
{
    pinMode(buttonPin, INPUT_PULLUP);
}

void ButtonHandler::setOnPressedCallback(std::function<void()> callback)
{
    onPressCallback = callback;
}

void ButtonHandler::setOnReleasedCallback(std::function<void()> callback)
{
    onReleaseCallback = callback;
}

void ButtonHandler::pinStateChanged()
{
    uint8_t state = digitalRead(buttonPin);
    bool newState = (state == LOW);
    if (isPressed == newState)
    {
        return;
    }
    if (newState)
    {
        if (onPressCallback)
            onPressCallback();
        pressEndTime = millis() + maxPressTime;
    }
    else
    {
        if (onReleaseCallback)
            onReleaseCallback();
    }
    isPressed = newState;
}

void ButtonHandler::update()
{
    if (isPressed && millis() >= pressEndTime)
    {
        isPressed = false;
        if (onReleaseCallback)
            onReleaseCallback();
    }
}

uint8_t ButtonHandler::getButtonPin()
{
    return buttonPin;
}
