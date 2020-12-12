//@group Core

#include <cstring>

#include "utf8.h"

namespace Engine
{
  // Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
  // See http: //bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

  static uint8_t const utf8d[] ={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
    0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
    0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
    0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
    1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
    1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
    1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
  };

  static uint32_t Decode(uint32_t *pState, uint32_t *pCodep, uint32_t byte)
  {
    uint32_t type = utf8d[byte];

    *pCodep = (*pState != UTF8_ACCEPT) ?
      (byte & 0x3fu) | (*pCodep << 6) :
      (0xff >> type) & (byte);

    *pState = utf8d[256 + *pState*16 + type];
    return *pState;
  }

  UTF8Parser::UTF8Parser()
    : m_remaining(0)
    , m_pText(nullptr)
  {

  }

  UTF8Parser::UTF8Parser(char const * a_pText)
    : m_remaining(strlen(a_pText))
    , m_pText((uint8_t*)a_pText)
  {

  }

  UTF8Parser::~UTF8Parser()
  {

  }

  void UTF8Parser::Init(char const * a_pText)
  {
    m_remaining = strlen(a_pText);
    m_pText = (uint8_t*)a_pText;
  }

  bool UTF8Parser::Done() const
  {
    return m_pText == nullptr;
  }

  UTF8CodePoint UTF8Parser::Next()
  {
    if (Done())
      return INVALID_CHAR;

    uint32_t codePoint = 0;
    uint32_t state = 0;
    size_t byteMin = m_remaining > 4 ? m_remaining - 4 : 0;

    for (; m_remaining > byteMin; m_remaining--, m_pText++)
    {
      if (Decode(&state, &codePoint, *m_pText) == UTF8_ACCEPT)
        break;
    }

    if (state == UTF8_ACCEPT)
    {
      m_remaining--;
      m_pText++;
    }

    if (m_remaining == 0 || state != UTF8_ACCEPT)
    {
      m_remaining = 0;
      m_pText = nullptr;
    }

    return state == UTF8_ACCEPT ? codePoint : INVALID_CHAR;
  }
}