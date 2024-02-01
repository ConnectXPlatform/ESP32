#pragma once
#include <Arduino.h>
#include <freertos/semphr.h>
#include <queue>
#include <vector>
#include <memory>

class Task
{
public:
    virtual void start() = 0;
    /// @brief Updates the task's state.
    /// @return true if the task is completed, and false otherwise.
    virtual bool update() = 0;
};

class TasksQueue
{
public:
    TasksQueue(TasksQueue const &) = delete;
    void operator=(TasksQueue const &) = delete;

    static TasksQueue &getInstance()
    {
        // Source: https://stackoverflow.com/a/1008289
        static TasksQueue instance;
        return instance;
    }

    void start(BaseType_t coreId = 1 - ARDUINO_RUNNING_CORE);

    void enqueueTask(std::unique_ptr<Task> task);

    void update()
    {
        processPendingTasks();
        removeCompletedTasks();
    }

private:
    SemaphoreHandle_t lock;
    std::queue<std::unique_ptr<Task>> pendingTasksQueue;
    std::vector<std::unique_ptr<Task>> executedTasks;

    TasksQueue()
    {
        lock = xSemaphoreCreateMutex();
    }

    void processPendingTasks();

    void removeCompletedTasks();

    static void taskUpdateHandler(void *parameter)
    {
        TasksQueue *instance = static_cast<TasksQueue *>(parameter);
        while (true)
        {
            instance->update();
            delay(1);
        }
    }
};
