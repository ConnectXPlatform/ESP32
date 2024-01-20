#pragma once
#include <functional>
#include "TasksQueue.h"

class BackgroundTask : public Task
{
public:
    BackgroundTask(std::function<void()> onStart, std::function<void()> onUpdate, unsigned long delayBetweenUpdates)
        : onStart(onStart), onUpdate(onUpdate), delayBetweenUpdates(delayBetweenUpdates)
    {
    }
    BackgroundTask(std::function<void()> onUpdate, unsigned long delayBetweenUpdates)
        : onStart(emptyFunction), onUpdate(onUpdate), delayBetweenUpdates(delayBetweenUpdates)
    {
    }

    void start() override
    {
        onStart();
    }
    bool update() override
    {
        if (millis() >= lastUpdateCall + delayBetweenUpdates)
        {
            onUpdate();
            lastUpdateCall = millis();
        }
        return false;
    }

private:
    std::function<void()> onStart, onUpdate;
    const unsigned long delayBetweenUpdates;
    unsigned long lastUpdateCall;

    static void emptyFunction() {}
};
