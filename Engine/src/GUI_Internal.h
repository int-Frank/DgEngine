//@group GUI

#ifndef GUI_INTERNAL_H
#define GUI_INTERNAL_H

#include "Utils.h"
#include "IFontAtlas.h"

namespace Engine
{
  namespace GUI
  {
    namespace Renderer
    {
      Dg::ErrorCode Init();
      void Destroy();

      void GetCharacterSizeRange(int16_t & ascent, int16_t & descent);

      // Get the glyph data for the default font and size
      GlyphData * GetGlyphData(CodePoint);
      void SetScreenSize(vec2 const &);
      void DrawBox(UIAABB const &, Colour colour);
      void DrawText(uint16_t textureID, Colour colour, uint32_t count, void * pData);
    }

    bool Intersection(UIAABB const & A, UIAABB const & B,  UIAABB & out);
    bool PointInBox(vec2 const & point, UIAABB const & box);
  }
}

#endif