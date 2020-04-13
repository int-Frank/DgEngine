#include "MemBuffer.h"


//--------------------------------------------------------------------------------------------
// MemBufferDynamic
//--------------------------------------------------------------------------------------------
MemBufferDynamic::MemBufferDynamic()
  : m_cursor(0)
  , m_memblock(s_defaultSize)
  , m_alignment(s_defaultAlignment)
{

}

MemBufferDynamic::MemBufferDynamic(size_t a_initialSize, size_t a_alignment)
  : m_cursor(0)
  , m_memblock(a_initialSize)
  , m_alignment(a_alignment)
{

}

MemBufferDynamic::~MemBufferDynamic()
{

}

MemBufferDynamic::MemBufferDynamic(MemBufferDynamic const& a_other)
  : m_cursor(a_other.m_cursor)
  , m_memblock(a_other.m_memblock)
  , m_alignment(a_other.m_alignment)
{

}

MemBufferDynamic& MemBufferDynamic::operator=(MemBufferDynamic const& a_other)
{
  if (this != &a_other)
  {
    m_cursor = a_other.m_cursor;
    m_memblock = a_other.m_memblock;
    m_alignment = a_other.m_alignment;
  }
  return *this;
}

typename MemBufferDynamic::Index
MemBufferDynamic::Allocate(size_t a_size)
{
  size_t newCursor = Dg::ForwardAlign<size_t>(m_cursor + a_size, m_alignment);
  if (newCursor > m_memblock.size())
    m_memblock.resize(newCursor);
  size_t oldCursor = m_cursor;
  m_cursor = newCursor;
  return oldCursor;
}

void* MemBufferDynamic::GetAddress(Index a_index)
{
  return m_memblock.data() + a_index;
}

void MemBufferDynamic::clear()
{
  m_cursor = 0;
}

size_t MemBufferDynamic::size() const
{
  return m_cursor;
}
//--------------------------------------------------------------------------------------------
// MemBuffer
//--------------------------------------------------------------------------------------------
MemBuffer::MemBuffer()
  : m_memblock(nullptr)
  , m_cursor(0)
  , m_alignment(s_defaultAlignment)
  , m_size(s_defaultSize)
{
  m_memblock = (unsigned char*)malloc(m_size);
  if (m_memblock == nullptr)
    throw std::exception("MemBuffer failed to allocate!");
}

MemBuffer::MemBuffer(size_t a_size)
  : m_memblock(nullptr)
  , m_cursor(0)
  , m_alignment(s_defaultAlignment)
  , m_size(a_size)
{
  m_memblock = (unsigned char*)malloc(m_size);
  if (m_memblock == nullptr)
    throw std::exception("MemBuffer failed to allocate!");
}

MemBuffer::MemBuffer(size_t a_size, size_t a_alignment)
  : m_memblock(nullptr)
  , m_cursor(0)
  , m_alignment(a_alignment)
  , m_size(a_size)
{
  m_memblock = (unsigned char*)malloc(m_size);
  if (m_memblock == nullptr)
    throw std::exception("MemBuffer failed to allocate!");
}

MemBuffer::~MemBuffer()
{
  free(m_memblock);
}

void* MemBuffer::Allocate(size_t a_size)
{
  BSR_ASSERT(m_cursor + a_size < m_size, "MemBuffer out of memory!");
  void* mem = &m_memblock[m_cursor];
  m_cursor += a_size;
  m_cursor = Dg::ForwardAlign<size_t>(m_cursor, m_alignment);
  return mem;
}

void MemBuffer::clear()
{
  m_cursor = 0;
}

size_t MemBuffer::size() const
{
  return m_cursor;
}