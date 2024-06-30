#pragma once

#include <cassert>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>


class ThreadPool 
{
public:
    ThreadPool() : ThreadPool(std::thread::hardware_concurrency())
    {
        
    }
    
    explicit ThreadPool(size_t size):
    _stop(false)
    {
        for (decltype(size) i = 0; i < size; ++i)
        {
            _workers.emplace_back(
                [this]
                {
                    while (true)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock lock(_mutex);
                            _cv.wait(lock, [this]{ return _stop || !_tasks.empty(); });
                            if (_stop && _tasks.empty())
                                return;
                            
                            task = std::move(_tasks.front());
                            _tasks.pop();
                        }
                        
                        if (task)
                            task();
                    }
                }
            );
        }
    }
    
    template<class TFunction, class... TArgs>
    auto AddTask(TFunction&& function, TArgs&&... args) -> std::future<typename std::invoke_result_t<TFunction, TArgs...>>
    {
        using Result = typename std::invoke_result_t<TFunction, TArgs...>;

        auto task = std::make_shared<std::packaged_task<Result()>>
        (
            std::bind(std::forward<TFunction>(function), std::forward<TArgs>(args)...)
        );

        std::future<Result> result = task->get_future();
        {
            std::unique_lock lock(_mutex);
            assert(!_stop);

            _tasks.emplace([task]()->void
            {
                (*task)();
            });
        }
        
        _cv.notify_one();
        return result;
    }
    
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _stop = true;
        }
        
        _cv.notify_all();
        
        for (auto& worker: _workers)
            worker.join();
    }
private:
    std::atomic<bool> _stop;
    std::mutex _mutex;
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::condition_variable _cv;
};
