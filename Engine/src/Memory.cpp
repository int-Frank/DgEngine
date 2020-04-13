//@group Memory

#include <mutex>
#include "MemBuffer.h"
#include "Memory.h"

namespace Engine
{
  namespace impl
  {
    namespace TRef
    {
      static std::mutex mutex;
      static MemBuffer buf(10 * 1024 * 1024);
    }
  }

  void TBUFClear()
  {
    impl::TRef::buf.clear();
  }

  void* TBUFAlloc(size_t a_size)
  {
    std::lock_guard<std::mutex> lock(impl::TRef::mutex);
    return impl::TRef::buf.Allocate(a_size);
  }
}