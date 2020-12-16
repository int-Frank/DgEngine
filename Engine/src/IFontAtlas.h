//@group Interface

#ifndef IFONTATLAS_H
#define IFONTATLAS_H

#include <string>

#include "DgError.h"
#include "unicode.h"
#include "Texture.h"
#include "Memory.h"

namespace Engine
{
  typedef uint32_t FontID;

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

    virtual Dg::ErrorCode RegisterFont(std::string const & fontPath, FontID & out) = 0;

    virtual void BeginLoad() = 0;
    virtual Dg::ErrorCode RegisterGlyph(FontID, uint32_t size, CodePoint c) = 0;
    virtual Dg::ErrorCode RegisterAllGlyphs(FontID, uint32_t size) = 0;
    virtual Dg::ErrorCode CommitLoad() = 0;

    virtual void Clear() = 0;
    virtual GlyphData * GetGlyphData(FontID, CodePoint cp, uint32_t size) = 0;
  };
}

#endif