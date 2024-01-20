#pragma once
#include <functional>

enum State : byte
{
    Pressed = 0,
    Released = 1
};
class ButtonHandler
{
public:
    ButtonHandler(const uint8_t buttonPin, const unsigned long maxPressTime);
    void init();
    void setOnPressedCallback(std::function<void()>);
    void setOnReleasedCallback(std::function<void()>);
    void pinStateChanged(); // Since we can't attach interrupt from inside the class, we need to call this from another script.
    void setState(State newState);
    void update();
    uint8_t getButtonPin();

private:
    const uint8_t buttonPin;
    const unsigned long maxPressTime;
    std::function<void()> onPressCallback;
    std::function<void()> onReleaseCallback;
    volatile unsigned long pressEndTime;
    State currentState;
};
