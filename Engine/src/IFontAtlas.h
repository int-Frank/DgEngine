//@group Interface

#ifndef IFONTATLAS_H
#define IFONTATLAS_H

#include <string>

#include "ErrorCodes.h"
#include "utf8.h"
#include "Texture.h"
#include "Memory.h"

namespace Engine
{
  typedef uint32_t FontID;
  typedef uint64_t GlyphID;

  class IFontAtlas
  {
  public:

    virtual ~IFontAtlas(){}

    virtual void Clear() = 0;
    virtual void BeginLoad() = 0;
    virtual FontID AddFont(std::string const & fontPath, uint32_t size) = 0;
    virtual void EndLoad() = 0;

    virtual GlyphID GetGlyphID(FontID, UTF8CodePoint c) = 0;
    //virtual Texture2D const * GetTexture() = 0;
  };
}

#endif