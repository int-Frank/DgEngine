#ifndef DOUBLEBUFFER_H
#define DOUBLEBUFFER_H

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "core_utils.h"
#include "PODArray.h"
#include "Membuffer.h"
#include "core_Assert.h"
#include "PODArray.h"

//A double buffer which supports many producer threads, and one consumer thread.
class DoubleBuffer
{
public:

  class Ref
  {
    friend class DoubleBuffer;
    Ref(std::atomic<size_t> *, void *);
  public:

    Ref();
    ~Ref();

    Ref(Ref const&);
    Ref& operator=(Ref const&);

    Ref(Ref &&);
    Ref& operator=(Ref &&);

    void * GetBuffer() const;

  private:
    std::atomic<size_t> * m_nProdRefs;
    void * m_buf;
  };

public:

  DoubleBuffer();
  DoubleBuffer(size_t size);
  DoubleBuffer(size_t size, size_t alignment);
  ~DoubleBuffer();

  DoubleBuffer(DoubleBuffer const &) = delete;
  DoubleBuffer& operator=(DoubleBuffer const &) = delete;

  //Producer: Can be called from any thread.
  //Allocates on the current producer buffer
  Ref Allocate(size_t);

  void Swap();

  //Consumer: Get the current consumer buffer
  MemBuffer & GetBuffer();

  //Get the number of Refs which currently have access to the producer buffer. 
  size_t GetCurrentRefCount();

private:

  int                     m_producerIndex;
  MemBuffer               m_buffer[2];
  std::atomic<size_t>     m_nProdRefs[2];
  std::mutex              m_mutex;
  std::condition_variable m_cv;
};

#endif