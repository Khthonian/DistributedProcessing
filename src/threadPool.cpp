// Copyright 2023 Stewart Charles Fisher II

#include "threadPool.h"

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
  for (std::thread& worker : workers) worker.join();
}
