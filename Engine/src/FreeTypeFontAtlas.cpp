//@group Framework

#include "IFontAtlas.h"
#include "DgMap_AVL.h"
#include "DgDynamicArray.h"
#include "DgBinPacker.h"
#include "Framework.h"

#define FONT_PATH "./fonts/"
#define FONT_NAME_MAX_SIZE 32

namespace Engine
{
  class FreeTypeFontAtlas : public IFontAtlas
  {
  public:

    FreeTypeFontAtlas();
    ~FreeTypeFontAtlas();

    void Clear() override;
    void BeginLoad() override;
    FontID AddFont(std::string const & path, uint32_t size, std::string const & name) override; //Check for FONT_NAME_MAX_SIZE 
    void EndLoad() override;
    SubBlock GetCharacterID(std::string const & name, uint32_t size, UTF8 c) override;
    Texture2D const * GetTexture() override;

  private:

    struct Font
    {
      std::string path;
      std::string name;
      uint32_t size;
    };

    struct TempData
    {
      Dg::DynamicArray<Font> fonts;
      Dg::BinPacker<uint32_t> layout;
    };

    class Char
    {

    public:

      bool operator<(Char const & a_other) const
      {
        if (c < a_other.c)
          return true;

        if (size < a_other.size)
          return true;

        if (nameHash < a_other.nameHash)
          return true;

        return false;
      }

      uint64_t nameHash;
      uint32_t size;
      UTF8 c;
    };

    Dg::Map_AVL<Char, SubBlock> m_charMap;
    Texture2D *m_pTexture;
  };

  void Framework::InitFontAtlas()
  {
    SetFontAtlas(new FreeTypeFontAtlas());
  }

  FreeTypeFontAtlas::FreeTypeFontAtlas()
    : m_pTexture(nullptr)
  {

  }

  FreeTypeFontAtlas::~FreeTypeFontAtlas()
  {
    delete m_pTexture;
  }

  void FreeTypeFontAtlas::Clear()
  {
  
  }

  void FreeTypeFontAtlas::BeginLoad()
  {

  }

  FontID FreeTypeFontAtlas::AddFont(std::string const & path, uint32_t size, std::string const & name)
  {
    return 0;
  }

  void FreeTypeFontAtlas::EndLoad()
  {

  }

  SubBlock FreeTypeFontAtlas::GetCharacterID(std::string const & name, uint32_t size, UTF8 c)
  {
    return SubBlock();
  }

  Texture2D const * FreeTypeFontAtlas::GetTexture()
  {
    return m_pTexture;
  }
}