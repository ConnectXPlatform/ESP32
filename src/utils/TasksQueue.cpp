#include "TasksQueue.h"

void TasksQueue::start(BaseType_t coreId)
{
    xTaskCreateUniversal(taskUpdateHandler, "tasks queue", getArduinoLoopTaskStackSize(), this, 1, NULL, coreId);
}

void TasksQueue::enqueueTask(std::unique_ptr<Task> task)
{
    std::lock_guard<std::mutex> _(lock);
    pendingTasksQueue.push(std::move(task));
}

void TasksQueue::processPendingTasks()
{
    // Copy all the tasks that needs to be executed to a temporary queue
    std::queue<std::unique_ptr<Task>> pendingTasks;
    {
        std::lock_guard<std::mutex> _(lock);
        // Move to the local queue
        while (!pendingTasksQueue.empty())
        {
            pendingTasks.push(std::move(pendingTasksQueue.front()));
            pendingTasksQueue.pop();
        }
    }
    // Execute the tasks
    while (pendingTasks.size() > 0)
    {
        std::unique_ptr<Task> waiting = std::move(pendingTasks.front());
        waiting->start();
        pendingTasks.pop();
        executedTasks.push_back(std::move(waiting));
    }
}

void TasksQueue::removeCompletedTasks()
{
    // Update the running tasks and delete the completed ones.
    for (auto it = executedTasks.begin(); it != executedTasks.end();)
    {
        if ((*it)->update())
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
