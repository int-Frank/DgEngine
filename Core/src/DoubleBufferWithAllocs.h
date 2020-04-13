#ifndef DOUBLEBUFFERWITHALLOCS_H
#define DOUBLEBUFFERWITHALLOCS_H

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "core_utils.h"
#include "PODArray.h"
#include "Membuffer.h"
#include "core_Assert.h"
#include "PODArray.h"

//A double buffer which supports many producer threads, and one consumer thread.
class DoubleBufferWithAllocs
{
public:

  class Ref
  {
    friend class DoubleBufferWithAllocs;
    Ref(std::atomic<size_t>*, void*);
  public:

    Ref();
    ~Ref();

    Ref(Ref const&);
    Ref& operator=(Ref const&);

    Ref(Ref&&);
    Ref& operator=(Ref&&);

    void* GetBuffer() const;

  private:
    std::atomic<size_t>* m_nProdRefs;
    void* m_buf;
  };

public:

  DoubleBufferWithAllocs();
  DoubleBufferWithAllocs(size_t size);
  DoubleBufferWithAllocs(size_t size, size_t alignment);
  ~DoubleBufferWithAllocs();

  DoubleBufferWithAllocs(DoubleBufferWithAllocs const&) = delete;
  DoubleBufferWithAllocs& operator=(DoubleBufferWithAllocs const&) = delete;

  //Producer: Can be called from any thread.
  //Allocates on the current producer buffer
  Ref Allocate(size_t);

  void Swap();

  //Consumer: Get the current consumer buffer
  MemBuffer& GetBuffer();

  //Consumer: Get the current allocation addresses
  PODArray<void*>& GetAllocations();

  //Get the number of Refs which currently have access to the producer buffer. 
  size_t GetCurrentRefCount();

private:

  int                     m_producerIndex;
  MemBuffer               m_buffer[2];
  PODArray<void*>        m_allocations[2];
  std::atomic<size_t>     m_nProdRefs[2];
  std::mutex              m_mutex;
  std::condition_variable m_cv;
};

#endif