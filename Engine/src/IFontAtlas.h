//@group Interface

#ifndef IFONTATLAS_H
#define IFONTATLAS_H

#include <string>

#include "DgError.h"
#include "unicode.h"
#include "Texture.h"
#include "Memory.h"
#include "Texture.h"

namespace Engine
{
  typedef uint32_t FontID;
  uint16_t const INVALID_FONT_TEXTURE = 0xFFFF;

  struct GlyphData
  {
    int16_t advance;
    uint16_t textureID;
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
    int16_t bearingX;
    int16_t bearingY;
  };

  class IFontAtlas
  {
  public:

    virtual ~IFontAtlas(){}

    static Dg::ErrorCode RegisterFont(std::string const & fontPath, FontID & out);
    static Dg::ErrorCode GetCharacterSizeRange(FontID, int16_t & ascent, int16_t & descent);

    virtual Dg::ErrorCode GetTexture(uint16_t, Ref<Texture2D> & out) = 0;
    virtual void SetTextureDimension(uint32_t) = 0;

    virtual void BeginLoad() = 0;
    virtual Dg::ErrorCode RegisterGlyph(FontID, uint32_t size, CodePoint c) = 0;
    virtual Dg::ErrorCode RegisterAllGlyphs(FontID, uint32_t size) = 0;
    virtual Dg::ErrorCode CommitLoad() = 0;

    virtual void Clear() = 0;
    virtual GlyphData * GetGlyphData(FontID, CodePoint cp, uint32_t size) = 0;
  };
}

#endif