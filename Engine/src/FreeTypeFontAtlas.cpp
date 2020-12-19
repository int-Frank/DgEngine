//@group Framework

#include <vector>
#include <set>
#include <algorithm>

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include "DgBinPacker.h"
#include "Framework.h"
#include "BSR_Assert.h"
#include "Options.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
  typedef uint64_t GlyphID;
  typedef Dg::BinPacker<uint32_t, GlyphID> BinPacker;

  static GlyphID PackGlyphID(FontID fID, CodePoint cp, uint32_t size)
  {
    BSR_ASSERT(fID < 0xFFFF, "Invalid font ID");
    BSR_ASSERT(size < 0xFFFF, "Invalid glyph size");
    return GlyphID(uint64_t(fID) << 48) | (uint64_t(size) << 32) | (uint64_t(cp));
  }

  static void UnpackGlyphID(GlyphID glyphID, FontID &fontID, CodePoint &cp, uint32_t &size)
  {
    fontID = FontID(glyphID >> 48);
    size = uint32_t((glyphID >> 32) & 0xFFFFul);
    cp = CodePoint(glyphID & 0xFFFF'FFFFul);
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

    Dg::ErrorCode GenerateCharMap();
    Dg::ErrorCode GenerateTextures();

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
    LoadGlyph lg = {};

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);
    DG_ERROR_IF((size_t)fID >= m_fonts.size(), Dg::ErrorCode::OutOfBounds);

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

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
      LoadGlyph lg = {};
      lg.cp = charcode;
      lg.size = size;

      m_pTempData->loadFonts[fID].glyphs.insert(lg);
      charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }

    result = Dg::ErrorCode::None;
  epilogue:
    FT_Done_Face(face);

    if (result != Dg::ErrorCode::None)
      LOG_ERROR("Failed to register glyphs from font '{}'. Error: {}", fID, Dg::ErrorCodeToString(result));

    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::GenerateCharMap()
  {
    Dg::ErrorCode result;
    FT_Error err;
    size_t fails = 0;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);

    for (size_t i = 0; i < m_pTempData->loadFonts.size(); i++)
    {
      FT_Face face = nullptr;
      err = FT_New_Face(m_pContext, m_fonts[i].c_str(), 0, &face);
      DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);
      DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);

      FT_Select_Charmap(face, FT_ENCODING_UNICODE);
      for (std::set<LoadGlyph>::iterator it = m_pTempData->loadFonts[i].glyphs.begin(); it != m_pTempData->loadFonts[i].glyphs.end();)
      {
        FT_Set_Pixel_Sizes(face, 0, it->size);

        bool hasFailed = true;
        do
        {
          if (FT_Load_Char(face, it->cp, FT_LOAD_RENDER) != 0)
            break;

          GlyphID glyphID = PackGlyphID(FontID(i), it->cp, it->size);

          GlyphData gData ={};
          gData.Advance = face->glyph->advance.x;
          gData.width = face->glyph->bitmap.width;
          gData.height = face->glyph->bitmap.rows;
          gData.bearingX = face->glyph->bitmap_left;
          gData.bearingY = face->glyph->bitmap_top;

          m_charMap.insert(glyphID, gData);

          hasFailed = false;
        } while (false);

        if (!hasFailed)
        {
          it++;
        }
        else
        {
          it = m_pTempData->loadFonts[i].glyphs.erase(it);
          fails++;
        }
      }
    }

    if (fails > 0)
      LOG_WARN("Failed to generate {} charactes when loading font", fails);

    result = Dg::ErrorCode::None;
  epilogue:
    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::GenerateTextures()
  {
    Dg::ErrorCode result;
    FT_Error err;
    size_t fails = 0;
    BinPacker binPacker;
    std::vector<FT_Face> fonts;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);

    for (size_t i = 0; i < m_fonts.size(); i++)
    {
      FT_Face face = nullptr;
      err = FT_New_Face(m_pContext, m_fonts[i].c_str(), 0, &face);
      DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);
      DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);
      FT_Select_Charmap(face, FT_ENCODING_UNICODE);
      fonts.push_back(face);
    }

    for (auto const & kv : m_charMap)
    {
      if (binPacker.RegisterItem(kv.first, kv.second.width, kv.second.height) != Dg::ErrorCode::None)
        fails++;
    }

    while (true)
    {
      BinPacker::Bin bin;
      bin.dimensions[0] = FONTATLAS_TEXTURE_DIMENSION;
      bin.dimensions[1] = FONTATLAS_TEXTURE_DIMENSION;
      bin.maxDimensions[0] = FONTATLAS_TEXTURE_DIMENSION;
      bin.maxDimensions[1] = FONTATLAS_TEXTURE_DIMENSION;

      size_t remaining = binPacker.Fill(bin);
      uint8_t * pBuffer = new uint8_t[FONTATLAS_TEXTURE_DIMENSION * FONTATLAS_TEXTURE_DIMENSION]{};

      for (auto item : bin.items)
      {
        auto it = m_charMap.find(item.id);
        DG_ERROR_IF(it == m_charMap.end(), Dg::ErrorCode::Failure);
        it->second.posX = item.xy[0];
        it->second.posY = item.xy[1];
        it->second.textureID = uint16_t(m_textures.size());

        FontID fontID;
        uint32_t size;
        CodePoint cp;
        UnpackGlyphID(it->first, fontID, cp, size);

        FT_Set_Pixel_Sizes(fonts[fontID], 0, size);
        if (FT_Load_Char(fonts[fontID], cp, FT_LOAD_RENDER) != 0)
        {
          fails++;
          continue;
        }

        for (int xLocal = 0; xLocal < it->second.width; xLocal++)
        {
          int x = item.xy[Dg::Element::x] + xLocal;
          for (int yLocal = 0; yLocal < it->second.height; yLocal++)
          {
            int y = item.xy[Dg::Element::y] + yLocal;
            uint8_t pixel = fonts[fontID]->glyph->bitmap.buffer[yLocal * fonts[fontID]->glyph->bitmap.width * yLocal + xLocal];
            pBuffer[y * FONTATLAS_TEXTURE_DIMENSION + x] = pixel;
          }
        }
      }

      Ref<Texture2D> texture = Texture2D::Create();
      TextureAttributes attrs;
      attrs.SetFilter(TextureFilter::Linear);
      attrs.SetIsMipmapped(false);
      attrs.SetWrap(TextureWrap::None);
      attrs.SetPixelType(TexturePixelType::R8);

      texture->Set(FONTATLAS_TEXTURE_DIMENSION, FONTATLAS_TEXTURE_DIMENSION, pBuffer, attrs);
      texture->Upload();

      m_textures.push_back(texture);

      if (remaining == 0)
        break;
    }

    for (FT_Face face : fonts)
      FT_Done_Face(face);

    LOG_INFO("FONT LOAD FAILS: {}", fails);

    result = Dg::ErrorCode::None;
  epilogue:

    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::CommitLoad()
  {
    Dg::ErrorCode result;
    FT_Error err;
    size_t fails = 0;
    BinPacker binPacker;

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);
    DG_ERROR_CHECK(GenerateCharMap());
    DG_ERROR_CHECK(GenerateTextures());

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
    auto it = m_charMap.find(PackGlyphID(fID, cp, size));
    if (it != m_charMap.end())
      return &(it->second);
    return nullptr;
  }
}