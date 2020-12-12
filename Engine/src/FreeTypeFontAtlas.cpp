//@group Framework

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include <vector>
#include "DgBinPacker.h"
#include "Framework.h"

#define FONT_PATH "./fonts/"
#define FONT_NAME_MAX_SIZE 32

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
    FontID AddFont(std::string const & path, uint32_t size) override;
    void EndLoad() override;
    GlyphID GetGlyphID(FontID, UTF8CodePoint c) override;

  private:

    struct InputFont
    {
      std::string path;
      uint32_t size;
    };

    struct TempData
    {
      std::vector<InputFont> fonts;
    };

    TempData * m_pInputData;

    //uint64_t = FontID << 32 | UTF8CodePoint
    Dg::Map_AVL<uint64_t, GlyphID> m_charMap;
    Dg::DynamicArray<Ref<Texture2D>> m_textures;
  };

  void Framework::InitFontAtlas()
  {
    SetFontAtlas(new FreeTypeFontAtlas());
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

  }

  FontID FreeTypeFontAtlas::AddFont(std::string const & path, uint32_t size)
  {
    return 0;
  }

  void FreeTypeFontAtlas::EndLoad()
  {

  }

  GlyphID FreeTypeFontAtlas::GetGlyphID(FontID, UTF8CodePoint c)
  {
    return 0;
  }
}