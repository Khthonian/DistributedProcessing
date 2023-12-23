// Copyright 2023 Stewart Charles Fisher II

// Include libraries
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#ifndef SRC_THREADPOOL_H_
#define SRC_THREADPOOL_H_

class ThreadPool {
 private:
  // Define a vector to store worker threads
  std::vector<std::thread> _workers_;
  
  // Define a queue to store tasks
  std::queue<std::function<void()>> _tasks_;

  // Define a mutex to protect task queue
  std::mutex _queueMutex_;
  
  // Define a condition variable for thread synchronisation
  std::condition_variable _condition_;
  
  // Define a flag to control the stopping of the thread pool
  bool _stop_;

 public:
  // Initialise the thread pool and start the threads
  ThreadPool(size_t threads);
  ~ThreadPool();

  // Define a template to enqueue a task into the thread pool
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;
};

// Include template
#include "threadPool.tpp"

#endif  // SRC_THREADPOOL_H_
