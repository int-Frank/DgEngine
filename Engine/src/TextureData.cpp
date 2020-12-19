//@group Renderer

#include "TextureData.h"
#include "Serialize.h"
#include "DgBit.h"

namespace Engine
{
  namespace
  {
    enum class Size : uint32_t
    {
      Wrap          = 2,
      Filter        = 2,
      MipmapFilter  = 2,
      IsMipmapped   = 1,
      PixelType     = 8
    };

    enum class Begin : uint32_t
    {
      Wrap          = 0,
      Filter        = Wrap + static_cast<uint32_t>(Size::Wrap),
      MipmapFilter  = Filter + static_cast<uint32_t>(Size::Filter),
      IsMipmapped   = MipmapFilter + static_cast<uint32_t>(Size::MipmapFilter),
      PixelType     = IsMipmapped + static_cast<uint32_t>(Size::IsMipmapped)
    };
  }

  size_t GetPixelSize(TexturePixelType a_type)
  {
    static size_t const s_sizes[] = 
    {
      1, // R8
      2, // RG8
      3, // RGB8
      4, // RGBA8
    };

    return s_sizes[static_cast<size_t>(a_type)];
  }

  //-----------------------------------------------------------------------------------------------
  // TextureAttributes
  //-----------------------------------------------------------------------------------------------

  TextureAttributes::TextureAttributes()
    : m_data(0)
  {
  
  }

  TextureWrap TextureAttributes::GetWrap() const
  {
    return static_cast<TextureWrap>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::Wrap), static_cast<uint32_t>(Size::Wrap)>(m_data));
  }

  TextureFilter TextureAttributes::GetFilter() const
  {
    return static_cast<TextureFilter>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::Filter), static_cast<uint32_t>(Size::Filter)>(m_data));
  }

  TextureMipmapFilter TextureAttributes::GetMipmapFilter() const
  {
    return static_cast<TextureMipmapFilter>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data));
  }

  TexturePixelType TextureAttributes::GetPixelType() const
  {
    return static_cast<TexturePixelType>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::PixelType), static_cast<uint32_t>(Size::PixelType)>(m_data));
  }

  bool TextureAttributes::IsMipmapped() const
  {
    return (Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::IsMipmapped), static_cast<uint32_t>(Size::IsMipmapped)>(m_data) != 0);
  }

  void TextureAttributes::SetWrap(TextureWrap a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::Wrap), static_cast<uint32_t>(Size::Wrap)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureAttributes::SetFilter(TextureFilter a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::Filter), static_cast<uint32_t>(Size::Filter)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureAttributes::SetMipmapFilter(TextureMipmapFilter a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureAttributes::SetPixelType(TexturePixelType a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::PixelType), static_cast<uint32_t>(Size::PixelType)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureAttributes::SetIsMipmapped(bool a_val)
  {
    uint32_t val = a_val ? 1 : 0;
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data, val);
  }

  uint32_t TextureAttributes::GetData() const
  {
    return m_data;
  }

  void TextureAttributes::SetData(uint32_t a_val)
  {
    m_data = a_val;
  }

  //-----------------------------------------------------------------------------------------------
  // TextureData
  //-----------------------------------------------------------------------------------------------

  TextureData::TextureData()
    : width(0)
    , height(0)
    , pPixels(nullptr)
  {

  }

  TextureData::~TextureData()
  {
    Clear();
  }

  TextureData::TextureData(uint32_t a_width, uint32_t a_height, void* a_pPixels, TextureAttributes a_attrs)
    : attrs(a_attrs)
    , width(a_width)
    , height(a_height)
    , pPixels(nullptr)
  {
    Set(a_width, a_height, a_pPixels, a_attrs);
  }

  TextureData::TextureData(TextureData const & a_other)
    : width(0)
    , height(0)
    , pPixels(nullptr)
  {
    Duplicate(a_other);
  }

  TextureData & TextureData::operator=(TextureData const & a_other)
  {
    if (this != &a_other)
      Duplicate(a_other);
    return *this;
  }

  TextureData::TextureData(TextureData && a_other) noexcept
    : attrs(a_other.attrs)
    , width(a_other.width)
    , height(a_other.height)
    , pPixels(a_other.pPixels)
  {
    a_other.width = 0;
    a_other.height = 0;
    a_other.pPixels = nullptr;
  }

  TextureData & TextureData::operator=(TextureData && a_other) noexcept
  {
    if (this != &a_other)
    {
      attrs = a_other.attrs;
      width = a_other.width;
      height = a_other.height;
      pPixels = a_other.pPixels;
      a_other.width = 0;
      a_other.height = 0;
      a_other.pPixels = nullptr;
    }

    return *this;
  }

  void TextureData::Set(uint32_t a_width, uint32_t a_height, void * a_pixels, TextureAttributes a_attrs)
  {
    Clear();
    width = a_width;
    height = a_height;
    attrs = a_attrs;
    pPixels = (uint8_t*)a_pixels;
  }

  void* TextureData::Serialize(void* a_pBuf)
  {
    void* pCurrent = a_pBuf;
    uint32_t attrData = attrs.GetData();
    pCurrent = ::Engine::Serialize(pCurrent, &attrData, 1);
    pCurrent = ::Engine::Serialize(pCurrent, &width, 1);
    pCurrent = ::Engine::Serialize(pCurrent, &height, 1);
    pCurrent = ::Engine::Serialize(pCurrent, pPixels, size_t(width) * height * GetPixelSize(attrs.GetPixelType()));
    return pCurrent;
  }

  void const * TextureData::Deserialize(void const * a_pBuf)
  {
    void const * pCurrent = a_pBuf;
    uint32_t attrData(0);
    pCurrent = ::Engine::Deserialize(pCurrent, &attrData, 1);
    attrs.SetData(attrData);

    pCurrent = ::Engine::Deserialize(pCurrent, &width, 1);
    pCurrent = ::Engine::Deserialize(pCurrent, &height, 1);
    pCurrent = ::Engine::Deserialize(pCurrent, pPixels, size_t(width) * height * GetPixelSize(attrs.GetPixelType()));
    return pCurrent;
  }

  void TextureData::Duplicate(TextureData const& a_other)
  {
    Clear();

    attrs = a_other.attrs;
    width = a_other.width;
    height = a_other.height;
    pPixels = new uint8_t[size_t(width) * height * GetPixelSize(attrs.GetPixelType())];
    memcpy(pPixels, a_other.pPixels, (size_t)width * height * GetPixelSize(attrs.GetPixelType()));
  }

  void TextureData::Clear()
  {
    attrs.SetData(0);
    width = 0;
    height = 0;
    delete[] pPixels;
    pPixels = nullptr;
  }

  size_t TextureData::Size() const
  {
    size_t result = 0;
    result += SerializedSize(attrs.GetData());
    result += SerializedSize(width);
    result += SerializedSize(height);
    result += (size_t(width) * height * GetPixelSize(attrs.GetPixelType()));
    return result;
  }
}