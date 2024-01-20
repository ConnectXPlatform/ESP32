#pragma once
#include <functional>
#include "TasksQueue.h"

class OneTimeTask : public Task
{
public:
    OneTimeTask(std::function<void()> onStart)
        : onStart(onStart)
    {
    }

    void start() override
    {
        onStart();
    }

    bool update() override
    {
        return true;
    }

private:
    std::function<void()> onStart;
};
