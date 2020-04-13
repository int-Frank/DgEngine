#ifndef GC_THREADPOOL_H
#define GC_THREADPOOL_H

#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <vector>

namespace GC
{
  class Job;

  class ThreadPool
  {
  public:

    ThreadPool();
    ThreadPool(size_t);
    ~ThreadPool();

    void Schedule(Job *);
    void FlagExitAndWait();
    void WaitAll();

    size_t ActiveJobs();
    size_t ThreadCount() const;

  private:

    void Init(size_t);
    void Handler();

  private:

    ThreadPool(ThreadPool const &) = delete;
    ThreadPool & operator=(ThreadPool const &) = delete;

  private:

    std::mutex                m_jobQueueMutex;
    std::queue<Job *>         m_jobQueue; //TODO Use Dg::List
    std::condition_variable   m_cv;
    std::vector<std::thread>  m_workerThreads;
    std::atomic<bool>         m_quit;
  };
}

#endif