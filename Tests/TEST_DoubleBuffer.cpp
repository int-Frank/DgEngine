#include <iostream>
#include <stdint.h>
#include "TestHarness.h"
#include "DoubleBuffer.h"

#define ALIGNMENT 16
typedef DoubleBuffer::Ref Ref;

TEST(Stack_DoubleBuffer, creation_DoubleBuffer)
{
  DoubleBuffer buffer(1024, ALIGNMENT);
  uint64_t root = 0;
  uint64_t buf1 = 0;
  CHECK(buffer.GetCurrentRefCount() == 0);
  {
    Ref ref = buffer.Allocate(ALIGNMENT);
    root = uint64_t(ref.GetBuffer());
    buf1 = root;
    CHECK(buffer.GetCurrentRefCount() == 1);
    ref = Ref();
    CHECK(buffer.GetCurrentRefCount() == 0);
  }
  CHECK(buffer.GetCurrentRefCount() == 0);

  {
    Ref ref = buffer.Allocate(ALIGNMENT);
    CHECK(uint64_t(ref.GetBuffer()) == root + ALIGNMENT);
    CHECK(buffer.GetCurrentRefCount() == 1);

    {
      Ref ref2 = ref;
      Ref ref3(ref2);
      CHECK(buffer.GetCurrentRefCount() == 3);
    }
    CHECK(buffer.GetCurrentRefCount() == 1);
  }
  CHECK(buffer.GetCurrentRefCount() == 0);

  buffer.Swap();
  CHECK(buffer.GetCurrentRefCount() == 0);
  {
    Ref ref = buffer.Allocate(ALIGNMENT);
    root = uint64_t(ref.GetBuffer());
    CHECK(root != buf1);
    CHECK(buffer.GetCurrentRefCount() == 1);
    ref = Ref();
    CHECK(buffer.GetCurrentRefCount() == 0);
  }
  CHECK(buffer.GetCurrentRefCount() == 0);

  {
    Ref ref = buffer.Allocate(ALIGNMENT);
    CHECK(uint64_t(ref.GetBuffer()) == root + ALIGNMENT);
    CHECK(buffer.GetCurrentRefCount() == 1);

    {
      Ref ref2 = ref;
      Ref ref3(ref2);
      CHECK(buffer.GetCurrentRefCount() == 3);
    }
    CHECK(buffer.GetCurrentRefCount() == 1);
  }
  CHECK(buffer.GetCurrentRefCount() == 0);
}