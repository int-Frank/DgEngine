//@group Interface

#ifndef IFONTATLAS_H
#define IFONTATLAS_H

#include <string>

#include "core_ErrorCodes.h"
#include "Texture.h"
#include "Memory.h"

namespace Engine
{
  typedef uint32_t UTF8;
  typedef uint32_t FontID;

  struct SubBlock
  {
    union
    {
      struct A
      {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
      } a;
      uint64_t d;
    };
  };

  class IFontAtlas
  {
  public:

    virtual ~IFontAtlas(){}

    virtual void Clear() = 0;
    virtual void BeginLoad() = 0;
    virtual FontID AddFont(std::string const & path, uint32_t size, std::string const & name = "") = 0;
    virtual void EndLoad() = 0;

    virtual SubBlock GetCharacterID(std::string const & name, uint32_t size, UTF8 c) = 0;
    virtual Texture2D const * GetTexture() = 0;
  };
}

#endif