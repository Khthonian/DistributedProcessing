// Copyright 2023 Stewart Charles Fisher II

// ThreadPool enqueue template
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    // Wrap the function and its arguments into a task
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // Get the future for the task's result
    std::future<return_type> res = task->get_future();
    {
        // Lock the mutex
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // If the pool has stopped, throw an exception
        if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

        // Add the task to the queue
        tasks.emplace([task]() { (*task)(); });
    }
    // Notify a waiting thread
    condition.notify_one();

    // Return the future
    return res;
}

