//@group GUI

#ifndef GUI_INTERNAL_H
#define GUI_INTERNAL_H

#include "Utils.h"
#include "IFontAtlas.h"

namespace DgE
{
  namespace GUI
  {
    namespace Renderer
    {
      Dg::ErrorCode Init();
      void Destroy();

      void GetCharacterSizeRange(uint32_t size, int16_t & ascent, int16_t & descent);

      // Get the glyph data for the default font and size
      GlyphData * GetGlyphData(CodePoint, uint32_t size);
      void SetScreenSize(vec2 const &);
      void DrawBox(UIAABB const &, Colour colour);
      void DrawBoxOutline(UIAABB const & inner, float thickness, Colour colour);
      void DrawBoxWithBorder(UIAABB const & inner, float thickness, Colour clrInner, Colour clrBorder);
      void DrawText(uint16_t textureID, Colour colour, uint32_t count, void * pVerts);
      Dg::ErrorCode DrawGlyph(CodePoint cp, uint32_t size, vec2 const & position, Colour colour);
    }

    bool Intersection(UIAABB const & A, UIAABB const & B,  UIAABB & out);
    bool PointInBox(vec2 const & point, UIAABB const & box);
  }
}

#endif