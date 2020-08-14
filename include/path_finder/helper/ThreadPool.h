#ifndef MASTER_ARBEIT_THREADPOOL_H
#define MASTER_ARBEIT_THREADPOOL_H
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <future>
namespace pathFinder {
class ThreadPool {
public:
  ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency());
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
}

#endif // MASTER_ARBEIT_THREADPOOL_H
