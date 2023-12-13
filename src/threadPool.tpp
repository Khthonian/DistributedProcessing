// ThreadPool Constructor
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    // Create the maximum number of worker threads
    for (size_t i = 0; i < threads; ++i)
        // Each worker thread executes this lambda function
        workers.emplace_back([this] {
            while (true) {
                // Declare a variable for the task
                std::function<void()> task;

                {
                    // Lock the mutex
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    // Wait until the next task or the pool is stopped
                    this->condition.wait(
                        lock, [this] { return this->stop || !this->tasks.empty(); });
                    // Exit the thread if not needed
                    if (this->stop && this->tasks.empty()) return;
                    // Take the next task from the queue
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // Execute the task
                task();
            }
        });
}

// ThreadPool Destructor
ThreadPool::~ThreadPool() {
    {
        // Lock the mutex
        std::unique_lock<std::mutex> lock(queueMutex);
        // Declare a stop flag
        stop = true;
    }
    // Notify all waiting threads
    condition.notify_all();
    // Join all worker threads to ensure they are completed before destruction
    for (std::thread& worker : workers)
        worker.join();
}

// ThreadPool enqueue method
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

