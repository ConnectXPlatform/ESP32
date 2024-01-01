#pragma once
#include <queue>
#include <vector>
#include <mutex>

class Task
{
public:
    const virtual void start() = 0;
    /// @brief Updates the task's state.
    /// @return true if the task is completed, and false otherwise.
    const virtual bool update() = 0;
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

    void enqueueTask(Task &task);

private:
    std::mutex lock;
    std::queue<std::reference_wrapper<Task>> pendingTasksQueue;
    std::vector<std::reference_wrapper<Task>> executedTasks;

    TasksQueue() {}

    void processPendingTasks();

    void removeCompletedTasks();

    static void taskUpdateHandler(void *parameter)
    {
        TasksQueue *instance = static_cast<TasksQueue *>(parameter);
        while (true)
        {
            instance->processPendingTasks();
            instance->removeCompletedTasks();
            delay(1);
        }
    }
};
