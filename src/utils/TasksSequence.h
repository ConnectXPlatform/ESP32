#pragma once
#include <queue>
#include <memory>
#include "TasksQueue.h"

class TasksSequence : public Task
{
public:
    TasksSequence() : tasks()
    {
    }

    void addTask(std::unique_ptr<Task> task)
    {
        tasks.push(std::move(task));
    }

    void start() override
    {
        if (tasks.size() == 0)
            return;
        tasks.front()->start();
    }

    bool update() override
    {
        if (tasks.size() == 0)
            return true;

        if (tasks.front()->update())
        {
            tasks.pop();
            if (tasks.size() == 0) // If this was the last task
                return true;
            tasks.front()->start();
        }
        return false;
    }

private:
    std::queue<std::unique_ptr<Task>> tasks;
};
