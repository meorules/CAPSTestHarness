#pragma once
#include <mutex>
#include <functional>
#include <queue>
#include <thread>
#include <functional>
#include <stdexcept>
#include <future>

class ThreadPool
{
public: 
  
  ThreadPool(int numberOfThreads= std::thread::hardware_concurrency());
  ~ThreadPool(){}
  template<class F, class... Args>
  auto QueueJob(F&& f, Args&&... args)
    ->std::future<typename std::invoke_result<F(Args...)>::type>;
  void Stop();

private:
  void ThreadLoop();

  bool terminate = false;
  int numberOfThreads;
  std::mutex queue_mutex;
  std::condition_variable cvJobQueue;
  std::queue<std::function<void()>> jobQueue;
  std::vector<std::thread> threads;

};

inline ThreadPool::ThreadPool(int numberOfThreads) {

  
  this-> numberOfThreads = numberOfThreads;
  threads.resize(numberOfThreads);
  for (int i = 0; i < numberOfThreads; i++) {
    threads.at(i) = std::thread(&ThreadPool::ThreadLoop);
  }
}

inline void ThreadPool::ThreadLoop() {
  std::function<void()> job;

  {std::unique_lock <std::mutex> lock(queue_mutex);
  while (jobQueue.size() < 1) {
    cvJobQueue.wait(lock);
  }

  if (jobQueue.size() > 1) {
    job = jobQueue.front();
    jobQueue.pop();
  }
  else if (terminate) {
    return;
  }
  job();
  }

}

template<class F, class... Args>
inline auto QueueJob(F&& f, Args&&... args)
->std::future<typename std::invoke_result<F(Args...)>::type> {

  using return_type = typename std::invoke_result<F(Args...)>::type;

  auto task = std::make_shared< std::packaged_task<return_type()> >(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

  std::future<return_type> res = task->get_future();

  if (!terminate) {
    std::unique_lock <std::mutex> lock(queue_mutex);
    
    jobQueue.emplace([task]() { (*task)(); });
    cvJobQueue.notify_one();
  }

  return res;

}

inline void ThreadPool::Stop() {
  {
    std::unique_lock <std::mutex> lock(queue_mutex);
    terminate = true;
  }
  cvJobQueue.notify_all();

  for (int i = 0; i < numberOfThreads; i++) {
    threads.at(i).join();
  }


}

