#ifndef MEMBUFFER_H
#define MEMBUFFER_H

#include <exception>
#include <cstdlib>
#include "DgMath.h"
#include "core_Assert.h"
#include "PODArray.h"
#include "core_utils.h"

class MemBufferDynamic
{
  static size_t const s_defaultSize = 1024;
  static size_t const s_defaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

public:

  typedef std::ptrdiff_t Index;

public:

  MemBufferDynamic();
  MemBufferDynamic(size_t initialSize, size_t alignment);
  ~MemBufferDynamic();

  MemBufferDynamic(MemBufferDynamic const& a_other);
  MemBufferDynamic& operator=(MemBufferDynamic const& a_other);

  Index Allocate(size_t);
  void * GetAddress(Index);
  void Write(byte *, size_t, Index);
  void clear();
  size_t size() const;

private:

  size_t          m_alignment;
  size_t          m_cursor;
  PODArray<byte>  m_memblock;
};

class MemBuffer
{
  static size_t const s_defaultSize = 1024;
  static size_t const s_defaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

private:

  MemBuffer(MemBuffer const&) = delete;
  MemBuffer& operator=(MemBuffer const&) = delete;

public:

  MemBuffer();
  MemBuffer(size_t size);
  MemBuffer(size_t size, size_t alignment);
  ~MemBuffer();

  void* Allocate(size_t a_size);
  void clear();
  size_t size() const;

private:

  size_t  m_size;
  size_t  m_alignment;
  size_t  m_cursor;
  byte *  m_memblock;
};

#endif