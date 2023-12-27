#pragma once
#include <vector>
#include "DisplayHandler.h"
#include "../input/sensors/TemperatureSensor.h"

class Screen
{
public:
    Screen(DisplayHandler &displayHandler) : displayHandler(displayHandler) {}

    void prepare();
    void dispose();

protected:
    DisplayHandler &displayHandler;

    virtual void prepareImpl() = 0;
    virtual void disposeImpl() = 0;
};

class SetupScreen : public Screen
{
public:
    void addStep(const char *step);

    void stepCompleted();

protected:
    void prepareImpl() override;
    void disposeImpl() override;

private:
    uint currentY;
    char *lastStep;
};
