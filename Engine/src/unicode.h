//@group Core

#ifndef UNICODE_H
#define UNICODE_H

#include <stdint.h>

namespace DgE
{
  typedef uint32_t CodePoint;
  CodePoint const INVALID_CHAR = 0xFFFF'FFFFul;

  class UTF8Parser
  {
  public:

    UTF8Parser();
    UTF8Parser(char const * pText);
    ~UTF8Parser();

    // Does not duplicate string!
    void Init(char const * pText);
    bool Done() const;
    CodePoint Next();

  private:

    size_t m_remaining;
    uint8_t const * m_pText;
  };
}

#endif