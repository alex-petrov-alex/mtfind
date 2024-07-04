#pragma once

#include <shared_mutex>
#include <queue>


template <class T>
class Queue
{
public:
    T Back()
    {
        std::shared_lock lock(_mutex);
        return _queue.back();
    }
    
    T Front()
    {
        std::shared_lock lock(_mutex);
        return _queue.front();
    }
    
    bool Empty()
    {
        std::shared_lock lock(_mutex);
        return _queue.empty();
    }
    
    size_t Size()
    {
        std::shared_lock lock(_mutex);
        return _queue.Size();
    }

    void Push(T&& value)
    {
        std::unique_lock lock(_mutex);
        _queue.push(std::forward<T>(value));
    };
    
    template <class ...TArgs>
    void Emplace(TArgs&& ...args)
    {
        std::unique_lock lock(_mutex);
        _queue.emplace(std::forward<TArgs>(args)...);
    };
    
    void Pop()
    {
        std::unique_lock lock(_mutex);
        _queue.pop();
    };
    
    void Clear()
    {
        std::unique_lock lock(_mutex);
        if (!_queue.empty())
            _queue.pop();
    };

private:
    std::queue<T> _queue;
    std::shared_mutex _mutex;
};
