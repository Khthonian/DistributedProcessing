// Copyright 2023 Stewart Charles Fisher II

#include "threadPool.h"

ThreadPool::ThreadPool(size_t threads) : _stop_(false) {
  // Create the maximum number of worker threads
  for (size_t i = 0; i < threads; ++i)
    // Each worker thread executes this lambda function
    _workers_.emplace_back([this] {
      while (true) {
        // Declare a variable for the task
        std::function<void()> task;

        {
          // Lock the mutex
          std::unique_lock<std::mutex> lock(this->_queueMutex_);
          // Wait until the next task or the pool is stopped
          this->_condition_.wait(
              lock, [this] { return this->_stop_ || !this->_tasks_.empty(); });
          // Exit the thread if not needed
          if (this->_stop_ && this->_tasks_.empty()) return;
          // Take the next task from the queue
          task = std::move(this->_tasks_.front());
          this->_tasks_.pop();
        }

        // Execute the task
        task();
      }
    });
}

ThreadPool::~ThreadPool() {
  {
    // Lock the mutex
    std::unique_lock<std::mutex> lock(_queueMutex_);
    // Declare a stop flag
    _stop_ = true;
  }
  // Notify all waiting threads
  _condition_.notify_all();
  // Join all worker threads to ensure they are completed before destruction
  for (std::thread& worker : _workers_) worker.join();
}
