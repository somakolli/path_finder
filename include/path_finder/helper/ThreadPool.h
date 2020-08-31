#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
namespace pathFinder {
class ThreadPool {
public:
  explicit ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency());
  void push(std::function<void()> function);
  void terminate();
  ~ThreadPool();

private:
  std::queue<std::function<void()>> m_taskQueue;
  size_t m_numberOfThreads;
  std::mutex queueMutex;
  std::vector<std::thread> threads;
  void threadLoop();
  bool m_terminate = false;
  std::condition_variable cv;
};
} // namespace pathFinder
