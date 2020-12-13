//@group Framework

#include <vector>

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include "DgBinPacker.h"
#include "Framework.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
  struct LoadGlyph
  {
    UTF8CodePoint cp;
    uint32_t size;
  };

  class LoadFont
  {
  public:

    void PushBack(LoadGlyph const &);

    std::string path;
    std::vector<LoadGlyph> glyphs;
  };

  class TempData
  {
  public:
    void PushBack(std::string const & path, LoadGlyph const &);
    std::vector<LoadFont> loadFonts;
  };

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

    TempData *m_pTempData;

    Dg::Map_AVL<GlyphID, GlyphData> m_charMap;
    std::vector<Ref<Texture2D>> m_textures;
  };

  void LoadFont::PushBack(LoadGlyph const & a_glyph)
  {
    bool found = false;
    for (auto const & item : glyphs)
    {
      if ((item.cp == a_glyph.cp) && (item.size == a_glyph.size))
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      glyphs.push_back(a_glyph);
    }
  }

  void TempData::PushBack(std::string const & a_path, LoadGlyph const & a_glyph)
  {
    bool found = false;
    std::string path = Framework::Instance()->GetFileSystem()->GetAbsolutePath(a_path);
    
    for (auto & loadFont : loadFonts)
    {
      if (loadFont.path == path)
      {
        loadFont.PushBack(a_glyph);
        found = true;
        break;
      }
    }

    if (!found)
    {
      LoadFont lf;
      lf.path = path;
      lf.glyphs.push_back(a_glyph);
      loadFonts.push_back(lf);
    }
  }

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
    //FT_Select_Charmap(face, ft_encoding_unicode); // Set utf8 encoding?

  epilogue:
    delete m_pTempData;
    FT_Done_FreeType(pFT);
    return result;
  }
}