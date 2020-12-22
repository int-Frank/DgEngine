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

uint8_t * g_pPixels_DEBUG = nullptr; // TODO DEBUG!!!

namespace Engine
{
  typedef uint64_t GlyphID;
  typedef Dg::BinPacker<uint32_t, GlyphID> BinPacker;

  static std::vector<std::string> s_FontPaths;

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

    void SetTextureDimension(uint32_t) override;

    void BeginLoad() override;
    Dg::ErrorCode RegisterGlyph(FontID, uint32_t size, CodePoint c) override;
    Dg::ErrorCode RegisterAllGlyphs(FontID, uint32_t size) override;
    Dg::ErrorCode CommitLoad() override;
    GlyphData * GetGlyphData(FontID, CodePoint cp, uint32_t size) override;

  private:

    Dg::ErrorCode GenerateCharMap();
    Dg::ErrorCode GenerateTextures();

    uint32_t m_textureDimension;
    FT_Library m_pContext;
    TempData *m_pTempData;
    Dg::Map_AVL<GlyphID, GlyphData> m_charMap;
    std::vector<Ref<Texture2D>> m_textures;
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
    : m_textureDimension(FONTATLAS_DEFAULT_TEXTURE_DIMENSION)
    , m_pContext(nullptr)
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

  void FreeTypeFontAtlas::SetTextureDimension(uint32_t a_uv)
  {
    m_textureDimension = a_uv;
  }

  Dg::ErrorCode IFontAtlas::RegisterFont(std::string const & fontPath, FontID & out)
  {
    Dg::ErrorCode result;
    FT_Face face = nullptr;
    FT_Error err;
    FT_Library pContext = nullptr;

    std::string path = Framework::Instance()->GetFileSystem()->GetAbsolutePath(fontPath);

    for (size_t i = 0; i < s_FontPaths.size(); i++)
    {
      if (path == s_FontPaths[i])
      {
        out = FontID(i);
        DG_ERROR_SET_AND_BREAK(Dg::ErrorCode::None);
      }
    }

    DG_ERROR_IF(FT_Init_FreeType(&pContext) != 0, Dg::ErrorCode::FailedToInitialise);

    err = FT_New_Face(pContext, path.c_str(), 0, &face);
    DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);
    DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);

    out = FontID(s_FontPaths.size());
    s_FontPaths.push_back(path);

    result = Dg::ErrorCode::None;
epilogue:
    FT_Done_Face(face);
    FT_Done_FreeType(pContext);

    if (result != Dg::ErrorCode::None)
      LOG_ERROR("Failed to register font. Error: {}", Dg::ErrorCodeToString(result));

    return result;
  }

  void FreeTypeFontAtlas::BeginLoad()
  {
    delete m_pTempData;
    m_pTempData = new TempData();
    m_textures.clear();

    for (uint32_t i = 0; i < (uint32_t)s_FontPaths.size(); i++)
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
    DG_ERROR_IF((size_t)fID >= s_FontPaths.size(), Dg::ErrorCode::OutOfBounds);

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
    DG_ERROR_IF((size_t)fID >= s_FontPaths.size(), Dg::ErrorCode::OutOfBounds);

    err = FT_New_Face(m_pContext, s_FontPaths[fID].c_str(), 0, &face);
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

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);

    for (size_t i = 0; i < m_pTempData->loadFonts.size(); i++)
    {
      FT_Face face = nullptr;
      err = FT_New_Face(m_pContext, s_FontPaths[i].c_str(), 0, &face);
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
          {
            LOG_WARN("Failed to read character: {}", it->cp);
            break;
          }

          GlyphID glyphID = PackGlyphID(FontID(i), it->cp, it->size);

          GlyphData gData ={};
          gData.textureID = GLYPH_NOT_RENDERABLE;
          gData.advance = (int16_t)(face->glyph->advance.x >> 6);
          gData.width = face->glyph->bitmap.width;
          gData.height = face->glyph->bitmap.rows;
          gData.bearingX = face->glyph->bitmap_left;
          gData.bearingY = face->glyph->bitmap_top;

          m_charMap.insert(Dg::Pair<GlyphID, GlyphData>(glyphID, gData));

          hasFailed = false;
        } while (false);

        if (!hasFailed)
        {
          it++;
        }
        else
        {
          it = m_pTempData->loadFonts[i].glyphs.erase(it);
        }
      }
    }

    result = Dg::ErrorCode::None;
  epilogue:
    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::GenerateTextures()
  {
    Dg::ErrorCode result;
    FT_Error err;
    BinPacker binPacker;
    std::vector<FT_Face> fonts;
    size_t previousRemaining = 0;
    size_t remaining = m_charMap.size();

    DG_ERROR_NULL(m_pTempData, Dg::ErrorCode::NullObject);

    for (size_t i = 0; i < s_FontPaths.size(); i++)
    {
      FT_Face face = nullptr;
      err = FT_New_Face(m_pContext, s_FontPaths[i].c_str(), 0, &face);
      DG_ERROR_IF(err == FT_Err_Unknown_File_Format, Dg::ErrorCode::IncorrectFileType);
      DG_ERROR_IF(err != 0, Dg::ErrorCode::FailedToOpenFile);
      FT_Select_Charmap(face, FT_ENCODING_UNICODE);
      fonts.push_back(face);
    }

    for (auto const & kv : m_charMap)
    {
      FontID fontID;
      uint32_t size;
      CodePoint cp;
      UnpackGlyphID(kv.first, fontID, cp, size);

      if (binPacker.RegisterItem(kv.first, kv.second.width, kv.second.height) != Dg::ErrorCode::None)
      {
        // White space...
      }
    }

    while ((remaining > 0) && (previousRemaining != remaining))
    {
      uint8_t * pBuffer = new uint8_t[m_textureDimension * m_textureDimension]{};
      g_pPixels_DEBUG = pBuffer; // TODO DEBUG!!!

      std::function<void(Dg::BinPacker<uint32_t, GlyphID>::Item const &)> callback = 
        [ &charMap = this->m_charMap,
          &textures = this->m_textures,
          &textureDimension = this->m_textureDimension,
          &fonts,
          &pBuffer ]
      (Dg::BinPacker<uint32_t, GlyphID>::Item const & item)
      {
        Dg::Map_AVL<GlyphID, GlyphData>::iterator it = charMap.find(item.id);
        
        FontID fontID;
        uint32_t size;
        CodePoint cp;
        UnpackGlyphID(item.id, fontID, cp, size);

        if (it == charMap.end())
        {
          LOG_WARN("Character missing from character map. Font: {}, size: {}, code point: {}", fontID, size, cp);
          return;
        }

        it->second.posX = (uint16_t)item.xy[0];
        it->second.posY = (uint16_t)item.xy[1];
        it->second.textureID = uint16_t(textures.size());

        FT_Set_Pixel_Sizes(fonts[fontID], 0, size);
        if (FT_Load_Char(fonts[fontID], cp, FT_LOAD_RENDER) != 0)
        {
          LOG_WARN("Failed to load character. Font: {}, size: {}, code point: {}", fontID, size, cp);
          return;
        }

        for (int xLocal = 0; xLocal < it->second.width; xLocal++)
        {
          int x = item.xy[0] + xLocal;
          for (int yLocal = 0; yLocal < it->second.height; yLocal++)
          {
            int y = item.xy[1] + yLocal;
            uint8_t pixel = fonts[fontID]->glyph->bitmap.buffer[yLocal * fonts[fontID]->glyph->bitmap.width + xLocal];
            pBuffer[y * textureDimension + x] = pixel;
          }
        }
      };

      previousRemaining = remaining;
      remaining = binPacker.Fill(callback, m_textureDimension, m_textureDimension);

      Ref<Texture2D> texture = Texture2D::Create();
      TextureAttributes attrs;
      attrs.SetFilter(TextureFilter::Linear);
      attrs.SetIsMipmapped(false);
      attrs.SetWrap(TextureWrap::None);
      attrs.SetPixelType(TexturePixelType::R8);

      texture->Set(m_textureDimension, m_textureDimension, pBuffer, attrs);
      texture->Upload(false); //TODO Should be true! flase for DEBUG

      m_textures.push_back(texture);
    }

    for (FT_Face face : fonts)
      FT_Done_Face(face);

    if (remaining > 0)
      LOG_WARN("Glyphs not loaded: {}", remaining);

    result = Dg::ErrorCode::None;
  epilogue:

    return result;
  }

  Dg::ErrorCode FreeTypeFontAtlas::CommitLoad()
  {
    Dg::ErrorCode result;

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
    Dg::Map_AVL<GlyphID, GlyphData>::iterator it = m_charMap.find(PackGlyphID(fID, cp, size));
    if (it != m_charMap.end())
      return &(it->second);
    return nullptr;
  }
}