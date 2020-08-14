#include <path_finder/helper/ThreadPool.h>

pathFinder::ThreadPool::ThreadPool(size_t numberOfThreads) {
  m_numberOfThreads = numberOfThreads;
  for(int i = 0; i < numberOfThreads; ++i)
    threads.emplace_back([&](){threadLoop();});
}

void pathFinder::ThreadPool::threadLoop() {
  while (!m_terminate) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lck(queueMutex);
      cv.wait(lck, [&](){return !m_taskQueue.empty() | m_terminate;});
      if(m_terminate && m_taskQueue.empty())
        return;
      task = m_taskQueue.front();
      m_taskQueue.pop();
    }
    task();
  }
}
void pathFinder::ThreadPool::terminate() {
  m_terminate = true;
  cv.notify_all();
  for(auto & thread: threads)
    thread.join();
  threads.clear();
}
pathFinder::ThreadPool::~ThreadPool() {
  terminate();
}
void pathFinder::ThreadPool::push(std::function<void()> function) {
  m_taskQueue.push(function);
  cv.notify_one();
}
