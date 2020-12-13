//@group Interface

#ifndef IFONTATLAS_H
#define IFONTATLAS_H

#include <string>

#include "DgError.h"
#include "utf8.h"
#include "Texture.h"
#include "Memory.h"

namespace Engine
{
  typedef uint64_t GlyphID;
  GlyphID const INVALID_GLYPHID = 0xFFFF'FFFF'FFFF'FFFFull;

  class IFontAtlas
  {
  public:

    virtual ~IFontAtlas(){}

    virtual void BeginLoad() = 0;
    virtual GlyphID RegisterGlyph(std::string const & fontPath, uint32_t size, UTF8CodePoint c) = 0;
    virtual Dg::ErrorCode CommitLoad() = 0;

    virtual void Clear() = 0;
    //virtual Texture2D const * GetTexture() = 0;
  };
}

#endif