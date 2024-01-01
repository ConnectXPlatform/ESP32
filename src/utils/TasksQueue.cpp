#include <Arduino.h>
#include "TasksQueue.h"

void TasksQueue::start(BaseType_t coreId)
{
    xTaskCreateUniversal(taskUpdateHandler, "tasks queue", getArduinoLoopTaskStackSize(), this, 1, NULL, coreId);
}

void TasksQueue::enqueueTask(Task &task)
{
    Serial.print(F("Adding task from core: "));
    Serial.println(xPortGetCoreID());
    std::lock_guard<std::mutex> _(lock);
    pendingTasksQueue.push(std::ref(task));
}

void TasksQueue::processPendingTasks()
{
    // Copy all the tasks that needs to be executed to a temporary queue
    std::queue<std::reference_wrapper<Task>> pendingTasks;
    {
        std::lock_guard<std::mutex> _(lock);
        if (pendingTasksQueue.empty())
            return;
        // Copy to the local queue
        pendingTasks = pendingTasksQueue;
        // Clear the tasks queue
        std::queue<std::reference_wrapper<Task>>().swap(pendingTasksQueue);
    }
    // Execute the tasks
    while (pendingTasks.size() > 0)
    {
        Task &waiting = pendingTasks.front();
        waiting.start();
        pendingTasks.pop();
        executedTasks.push_back(waiting);
    }
}

void TasksQueue::removeCompletedTasks()
{
    // Update the running tasks and delete the completed ones.
    for (auto it = executedTasks.begin(); it != executedTasks.end();)
    {
        Task &t = *it;
        if (t.update())
        {
            // Erase the element from the deque
            it = executedTasks.erase(it);
        }
        else
        {
            it++;
        }
    }
}
