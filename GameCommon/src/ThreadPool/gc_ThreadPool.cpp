
#include "gc_ThreadPool.h"
#include "gc_Job.h"

namespace GC
{
  void ThreadPool::Handler()
  {
    std::unique_lock<std::mutex> lock(m_jobQueueMutex);

    while (!m_quit)
    {
      m_cv.wait(lock, [this]
        { 
          return (m_jobQueue.size() != 0) || m_quit;
        });

      if (m_quit)
      {
        lock.unlock();
        break;
      }

      if (m_jobQueue.size() != 0)
      {
        Job * job = m_jobQueue.front();
        m_jobQueue.pop();
        lock.unlock();
        job->Run();
        job->Done();
        delete job;
        lock.lock();
      }
    }
  }

  ThreadPool::ThreadPool()
    : m_quit(false)
  {
    Init(std::thread::hardware_concurrency());
  }

  ThreadPool::ThreadPool(size_t a_nThreads)
    : m_quit(false)
  {
    Init(a_nThreads);
  }

  void ThreadPool::Init(size_t a_nThreads)
  {
    if (a_nThreads == 0)
      a_nThreads = 1;
    for (size_t i = 0; i < a_nThreads; i++)
    {
      m_workerThreads.push_back(std::thread(&ThreadPool::Handler, this));
    }
  }

  ThreadPool::~ThreadPool()
  {
    FlagExitAndWait();
  }

  void ThreadPool::Schedule(Job * a_job)
  {
    std::unique_lock<std::mutex> lock(m_jobQueueMutex);
    m_jobQueue.push(a_job);
    lock.unlock();
    m_cv.notify_one();
  }

  void ThreadPool::FlagExitAndWait()
  {
    m_quit = true;
    m_cv.notify_all();

    for (auto & t : m_workerThreads)
      t.join();

    while (m_jobQueue.size() > 0)
    {
      delete m_jobQueue.front();
      m_jobQueue.pop();
    }
  }

  void ThreadPool::WaitAll()
  {
    while (ActiveJobs() != 0) 
      std::this_thread::yield();
  }

  size_t ThreadPool::ThreadCount() const
  {
    return m_workerThreads.size();
  }

  size_t ThreadPool::ActiveJobs()
  {
    std::lock_guard<std::mutex> lock(m_jobQueueMutex);
    return m_jobQueue.size();
  }
}

