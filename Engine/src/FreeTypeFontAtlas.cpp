//@group Framework

#include <vector>
#include <set>
#include <algorithm>

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include "DgBinPacker.h"
#include "Framework.h"
#include "BSR_Assert.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
  typedef uint64_t GlyphID;
  static GlyphID PackKey(FontID fID, CodePoint cp, uint32_t size)
  {
    BSR_ASSERT(fID < 0xFFFF, "Invalid font ID");
    BSR_ASSERT(size < 0xFFFF, "Invalid glyph size");
    return GlyphID(uint64_t(fID) << 48) | (uint64_t(size) << 32) | (uint64_t(cp));
  }

  struct LoadGlyph
  {
    CodePoint cp;
    uint32_t size;
  };

  struct LoadFont
  {
    std::set<LoadGlyph> glyphs;
  };

  struct TempData
  {
    std::vector<LoadFont> loadFonts;
  };

  class FreeTypeFontAtlas : public IFontAtlas
  {
  public:

    FreeTypeFontAtlas();
    ~FreeTypeFontAtlas();

    void Clear() override;
    Dg::ErrorCode Init();

    Dg::ErrorCode RegisterFont(std::string const & fontPath, FontID & out);

    void BeginLoad() override;
    Dg::ErrorCode RegisterGlyph(FontID, uint32_t size, CodePoint c) override;
    Dg::ErrorCode RegisterAllGlyphs(FontID, uint32_t size) override;
    Dg::ErrorCode CommitLoad() override;
    GlyphData * GetGlyphData(FontID, CodePoint cp, uint32_t size) override;

  private:

    FT_Library m_pContext;
    TempData *m_pTempData;
    Dg::Map_AVL<GlyphID, GlyphData> m_charMap;
    std::vector<Ref<Texture2D>> m_textures;
    std::vector<std::string> m_fonts;
  };

  Ref<IFontAtlas> Framework::CreateFontAtlas()
  {
    FreeTypeFontAtlas * ptr = new FreeTypeFontAtlas();

    if (ptr->Init() != Dg::ErrorCode::None)
    {
      delete ptr;
      ptr = nullptr;
    }

    return Ref<IFontAtlas>(ptr);
  }

  bool operator<(LoadGlyph const & a, LoadGlyph const & b)
  {
    if (a.size == b.size)
      return a.cp < b.cp;
    return a.size < b.size;
  }

  FreeTypeFontAtlas::FreeTypeFontAtlas()
    : m_pContext(nullptr)
    , m_pTempData(nullptr)
  {

  }

  FreeTypeFontAtlas::~FreeTypeFontAtlas()
  {
    delete m_pTempData;
    FT_Done_FreeType(m_pContext);
  }

  Dg::ErrorCode FreeTypeFontAtlas::Init()
  {
    return FT_Init_FreeType(&m_pContext) == 0 ? Dg::ErrorCode::None : Dg::ErrorCode::FailedToInitialise;
  }

  void FreeTypeFontAtlas::Clear()
  {
    delete m_pTempData;
    m_pTempData = nullptr;
    m_textures.clear();
  }

  Dg::ErrorCode FreeTypeFontAtlas::RegisterFont(std::string const & fontPath, FontID & out)
  {
    Dg::ErrorCode result;
    FT_Face face = nullptr;
    FT_ULong charcode;
    FT_UInt gindex;
    uint32_t charCount = 0;
    FT_Error err;

    std::string path = Framework::Instance()->GetFileSystem()->GetAbsolutePath(fontPath);

    for (size_t i = 0; i < m_fonts.size(); i++)
    {
      if (path == m_fonts[i])
      {
        out = FontID(i);
        DG_ERROR_SET(Dg::ErrorCode::None);
      }
    }

    err = FT_New_Face(m_pContext, path.c_str(), 0, &face);
    DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);
    DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);

    //FT_Select_Charmap(face, ft_encoding_unicode);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
      charCount++;
      charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }

    out = FontID(m_fonts.size());
    m_fonts.push_back(path);
    LOG_INFO("Font '{}' contains {} characters.", fontPath.c_str(), charCount);

    result = Dg::ErrorCode::None;
epilogue:
    FT_Done_Face(face);

    if (result != Dg::ErrorCode::None)
      LOG_ERROR("Failed to register font. Error: {}", Dg::ErrorCodeToString(result));

    return result;
  }

  void FreeTypeFontAtlas::BeginLoad()
  {
    delete m_pTempData;
    m_pTempData = new TempData();
    m_textures.clear();

    for (uint32_t i = 0; i < (uint32_t)m_fonts.size(); i++)
    {
      LoadFont lf;
      m_pTempData->loadFonts.push_back(lf);
    }
  }

  Dg::ErrorCode FreeTypeFontAtlas::RegisterGlyph(FontID fID, uint32_t size, CodePoint cp)
  {
    Dg::ErrorCode result;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);
    DG_ERROR_IF((size_t)fID >= m_fonts.size(), Dg::ErrorCode::OutOfBounds);

    LoadGlyph lg;
    lg.cp = cp;
    lg.size = size;
    
    m_pTempData->loadFonts[fID].glyphs.insert(lg);

    result = Dg::ErrorCode::None;
  epilogue:
    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::RegisterAllGlyphs(FontID fID, uint32_t size)
  {
    Dg::ErrorCode result;
    FT_Face face = nullptr;
    FT_ULong charcode;
    FT_UInt gindex;
    FT_Error err;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);
    DG_ERROR_IF((size_t)fID >= m_fonts.size(), Dg::ErrorCode::OutOfBounds);

    err = FT_New_Face(m_pContext, m_fonts[fID].c_str(), 0, &face);
    DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);
    DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);

    //FT_Select_Charmap(face, ft_encoding_unicode);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
      LoadGlyph lg;
      lg.cp = charcode;
      lg.size = size;

      m_pTempData->loadFonts[fID].glyphs.insert(lg);
      charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }

    result = Dg::ErrorCode::None;
  epilogue:
    FT_Done_Face(face);

    if (result != Dg::ErrorCode::None)
      LOG_ERROR("Failed to load all glyphs from font '{}'. Error: {}", fID, Dg::ErrorCodeToString(result));

    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::CommitLoad()
  {
    Dg::ErrorCode result;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);

    // Load glyphs...
    // Try to group fonts on one texture
    //FT_Select_Charmap(face, ft_encoding_unicode);

    result = Dg::ErrorCode::None;
  epilogue:

    delete m_pTempData;
    m_pTempData = nullptr;

    if (result != Dg::ErrorCode::None)
      LOG_ERROR("Failed to commit font atlas. Error: {}", Dg::ErrorCodeToString(result));

    return result;
  }

  GlyphData * FreeTypeFontAtlas::GetGlyphData(FontID fID, CodePoint cp, uint32_t size)
  {
    auto it = m_charMap.find(PackKey(fID, cp, size));
    if (it != m_charMap.end())
      return &(it->second);
    return nullptr;
  }
}