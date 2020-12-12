//@group Core

#ifndef UTF8_H
#define UTF8_H

#include <stdint.h>

namespace Engine
{
  typedef uint32_t UTF8CodePoint;
  UTF8CodePoint const INVALID_CHAR = 0xFFFF'FFFFul;

  class UTF8Parser
  {
  public:

    UTF8Parser();
    UTF8Parser(char const * pText);
    ~UTF8Parser();

    // Does not duplicate string!
    void Init(char const * pText);
    bool Done() const;
    UTF8CodePoint Next();

  private:

    size_t m_remaining;
    uint8_t const * m_pText;
  };
}

#endif