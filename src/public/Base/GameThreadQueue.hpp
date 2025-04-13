#pragma once
#include "Base/Core.h"
#include <queue>
#include <mutex>
#include <functional>

class GameThreadQueue
{
    std::mutex m_TaskMutex;
	std::queue<std::function<void()>> m_Tasks;

public:
    template<typename F>
    void Enqueue(F&& task) 
    {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        m_Tasks.emplace(std::forward<F>(task));
    }

    void ProcessTasks() 
    {
        std::queue<std::function<void()>> Tasks;

        {
            std::lock_guard<std::mutex> lock(m_TaskMutex);
            Tasks.swap(m_Tasks);
        }

        while (!Tasks.empty()) 
        {
            Tasks.front()();
            Tasks.pop();
        }
    }
};