//@group Framework

#include <vector>

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include "DgBinPacker.h"
#include "Framework.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/*
  GlyphID bits:
  16: 65536 x
  16: 65536 y
  10: 1024  width
  10: 1024  height
  12: 4096  texture Index
*/

namespace Engine
{
  class FreeTypeFontAtlas : public IFontAtlas
  {
  public:

    FreeTypeFontAtlas();
    ~FreeTypeFontAtlas();

    void Clear() override;

    void BeginLoad() override;
    GlyphID RegisterGlyph(std::string const & fontPath, uint32_t size, UTF8CodePoint c) override;
    Dg::ErrorCode CommitLoad() override;

  private:

    struct GlyphData
    {
      int32_t Advance;
      uint16_t textureIndex;
      int16_t posX;
      int16_t posY;
      int16_t width;
      int16_t height;
      int16_t bearingX;
      int16_t bearingY;
    };

    struct TempData
    {
      // 16: Font index
      // 16: size
      // 32: UTF8CodePoint
      std::vector<uint64_t> sizedCodePoints;
      std::vector<std::string> fontPaths;
    };

    TempData *pTempData;

    Dg::Map_AVL<GlyphID, GlyphData> m_charMap;
    std::vector<Ref<Texture2D>> m_textures;
  };

  Dg::ErrorCode Framework::InitFontAtlas()
  {
    SetFontAtlas(new FreeTypeFontAtlas());
    return Dg::ErrorCode::None;
  }

  FreeTypeFontAtlas::FreeTypeFontAtlas()
  {

  }

  FreeTypeFontAtlas::~FreeTypeFontAtlas()
  {

  }

  void FreeTypeFontAtlas::Clear()
  {
  
  }

  void FreeTypeFontAtlas::BeginLoad()
  {
    Clear();
  }

  GlyphID FreeTypeFontAtlas::RegisterGlyph(std::string const & fontPath, uint32_t size, UTF8CodePoint c)
  {
    return 0;
  }

  Dg::ErrorCode FreeTypeFontAtlas::CommitLoad()
  {
    FT_Library pFT = nullptr;
    Dg::ErrorCode result = Dg::ErrorCode::None;

    if (FT_Init_FreeType(&pFT) != 0)
    {
      LOG_ERROR("Failed to initialise FreeType library.");
      DG_ERROR_SET(Dg::ErrorCode::Failure);
    }

    // Load glyphs...


  epilogue:
    FT_Done_FreeType(pFT);
    return result;
  }
}