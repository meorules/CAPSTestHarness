#pragma once
#include <mutex>
#include <vector>
#include <functional>
#include <queue>
#include <thread>
#include <functional>
#include <stdexcept>
#include <future>
#include "TCPClient.h"

#define DEFAULT_PORT 12345


class ThreadPool
{
public: 
  
  ThreadPool(std::string serverIP,int numberOfThreads= std::thread::hardware_concurrency());
  ~ThreadPool(){}
  auto QueueJob(std::function<void()> job);


  template <class F, class... Args>
  std::future<std::invoke_result_t<F(Args...)>> QueueJob(F&& f, Args &&...args);

  void Stop();

private:
  void ThreadLoop();

  bool terminate = false;
  int numberOfThreads;
  std::mutex queue_mutex;
  std::condition_variable cvJobQueue;
  std::queue<std::function<void()>> jobQueue;
  std::vector<std::thread> threads;
  std::string serverIP;

};

inline ThreadPool::ThreadPool(string serverIP, int numberOfThreads) : serverIP(serverIP),numberOfThreads(numberOfThreads) {
  threads.resize(numberOfThreads);
  for (int i = 0; i < numberOfThreads; i++) {
    threads.at(i) = std::thread(&ThreadPool::ThreadLoop);
  }
}

inline void ThreadPool::ThreadLoop() {

  TCPClient client(serverIP, DEFAULT_PORT);

  client.OpenConnection();

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

//template<class F, class... Args>
//inline auto ThreadPool::QueueJob(F&& f, Args&&... args)
//->std::future<typename std::invoke_result<F(Args...)>::type> {
//
//  using return_type = typename std::invoke_result<F(Args...)>::type;
//
//  auto task = make_shared<packaged_task<return_type()> >(
//    bind(forward<F>(f), forward<Args>(args)...)
//    );
//
//  std::future<return_type> res = task->get_future();
//
//  if (!terminate) {
//    std::unique_lock <std::mutex> lock(queue_mutex);
//    
//    jobQueue.emplace([task]() { (*task)(); });
//    cvJobQueue.notify_one();
//  }
//
//  return res;
//
//}

inline auto ThreadPool::QueueJob(std::function<void()> job) {
  if (!terminate) {
        std::unique_lock <std::mutex> lock(queue_mutex);
        jobQueue.emplace(job);
        cvJobQueue.notify_one();
      }
}


template <class F, class... Args>
std::future<std::invoke_result_t<F(Args...)>> ThreadPool::QueueJob(F&& f, Args &&...args)
{
  /* The return type of task `F` */
  using return_type = std::invoke_result_t<F(Args...)>;

  /* wrapper for no arguments */
  auto task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock lock(queue_mutex);

    if (terminate)
      throw std::runtime_error("The thread pool has been stop.");

    /* wrapper for no returned value */
    jobQueue.emplace([task]() -> void { (*task)(); });
  }
  cvJobQueue.notify_one();
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

