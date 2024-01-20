#include <Arduino.h>
#include "ButtonHandler.h"

#define digital_state_to_button_state(digitalState) (digitalState) == LOW ? State::Pressed : State::Released

ButtonHandler::ButtonHandler(const uint8_t buttonPin, const unsigned long maxPressTime)
    : buttonPin(buttonPin), maxPressTime(maxPressTime), onPressCallback(nullptr), onReleaseCallback(nullptr), currentState(State::Released)
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
    State state = digital_state_to_button_state(digitalRead(buttonPin));
    setState(state);
}

void ButtonHandler::setState(State newState)
{
    if (newState == currentState)
        return;

    if (newState == State::Pressed)
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
    currentState = newState;
}

void ButtonHandler::update()
{
    if (currentState == State::Pressed && millis() >= pressEndTime)
    {
        setState(State::Released);
    }
}

uint8_t ButtonHandler::getButtonPin()
{
    return buttonPin;
}
