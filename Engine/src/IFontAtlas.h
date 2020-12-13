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
  // GlyphID is just the index to the glyph in the charMap
  typedef uint32_t GlyphID;
  GlyphID const INVALID_GLYPHID = 0xFFFF'FFFFul;

  struct GlyphData
  {
    int32_t Advance;
    uint16_t textureID;
    int16_t posX;
    int16_t posY;
    int16_t width;
    int16_t height;
    int16_t bearingX;
    int16_t bearingY;
  };

  class IFontAtlas
  {
  public:

    virtual ~IFontAtlas(){}

    virtual void BeginLoad() = 0;
    virtual GlyphID RegisterGlyph(std::string const & fontPath, uint32_t size, UTF8CodePoint c) = 0;
    virtual Dg::ErrorCode CommitLoad() = 0;

    virtual void Clear() = 0;
    //virtual GlyphData * GetGlyphData(GlyphID);
  };
}

#endif