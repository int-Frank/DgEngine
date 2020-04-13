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
      IsMipmapped   = 1
    };

    enum class Begin : uint32_t
    {
      Wrap          = 0,
      Filter        = Wrap + static_cast<uint32_t>(Size::Wrap),
      MipmapFilter  = Filter + static_cast<uint32_t>(Size::Filter),
      IsMipmapped   = MipmapFilter + static_cast<uint32_t>(Size::MipmapFilter),
    };
  }

  //-----------------------------------------------------------------------------------------------
  // TextureFlags
  //-----------------------------------------------------------------------------------------------

  TextureFlags::TextureFlags()
    : m_data(0)
  {
  
  }

  TextureWrap TextureFlags::GetWrap() const
  {
    return static_cast<TextureWrap>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::Wrap), static_cast<uint32_t>(Size::Wrap)>(m_data));
  }

  TextureFilter TextureFlags::GetFilter() const
  {
    return static_cast<TextureFilter>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::Filter), static_cast<uint32_t>(Size::Filter)>(m_data));
  }

  TextureMipmapFilter TextureFlags::GetMipmapFilter() const
  {
    return static_cast<TextureMipmapFilter>(Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data));
  }

  bool TextureFlags::IsMipmapped() const
  {
    return (Dg::GetSubInt<uint32_t, static_cast<uint32_t>(Begin::IsMipmapped), static_cast<uint32_t>(Size::IsMipmapped)>(m_data) != 0);
  }

  void TextureFlags::SetWrap(TextureWrap a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::Wrap), static_cast<uint32_t>(Size::Wrap)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureFlags::SetFilter(TextureFilter a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::Filter), static_cast<uint32_t>(Size::Filter)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureFlags::SetMipmapFilter(TextureMipmapFilter a_val)
  {
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data, static_cast<uint32_t>(a_val));
  }

  void TextureFlags::SetIsMipmapped(bool a_val)
  {
    uint32_t val = a_val ? 1 : 0;
    m_data = Dg::SetSubInt<uint32_t, static_cast<uint32_t>(Begin::MipmapFilter), static_cast<uint32_t>(Size::MipmapFilter)>(m_data, val);
  }

  uint32_t TextureFlags::GetData() const
  {
    return m_data;
  }

  void TextureFlags::SetData(uint32_t a_val)
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

  TextureData::TextureData(uint32_t a_width, uint32_t a_height, RGBA* a_pPixels, TextureFlags a_flags)
    : flags(a_flags)
    , width(a_width)
    , height(a_height)
    , pPixels(nullptr)
  {
    Set(a_width, a_height, a_pPixels, a_flags);
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
    : flags(a_other.flags)
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
      flags = a_other.flags;
      width = a_other.width;
      height = a_other.height;
      pPixels = a_other.pPixels;
      a_other.width = 0;
      a_other.height = 0;
      a_other.pPixels = nullptr;
    }

    return *this;
  }

  void TextureData::Set(uint32_t a_width, uint32_t a_height, RGBA * a_pixels, TextureFlags a_flags)
  {
    Clear();
    width = a_width;
    height = a_height;
    flags = a_flags;
    pPixels = a_pixels;
  }

  void* TextureData::Serialize(void* a_pBuf)
  {
    void* pCurrent = a_pBuf;
    uint32_t flagData = flags.GetData();
    pCurrent = Core::Serialize(pCurrent, &flagData, 1);
    pCurrent = Core::Serialize(pCurrent, &width, 1);
    pCurrent = Core::Serialize(pCurrent, &height, 1);
    pCurrent = Core::Serialize(pCurrent, &pPixels->data, size_t(width) * height);
    return pCurrent;
  }

  void const * TextureData::Deserialize(void const * a_pBuf)
  {
    void const * pCurrent = a_pBuf;
    uint32_t flagData(0);
    pCurrent = Core::Deserialize(pCurrent, &flagData, 1);
    pCurrent = Core::Deserialize(pCurrent, &width, 1);
    pCurrent = Core::Deserialize(pCurrent, &height, 1);
    pCurrent = Core::Deserialize(pCurrent, &pPixels->data, size_t(width) * height);
    flags.SetData(flagData);
    return pCurrent;
  }

  void TextureData::Duplicate(TextureData const& a_other)
  {
    Clear();

    flags = a_other.flags;
    width = a_other.width;
    height = a_other.height;
    pPixels = new RGBA[width * height];
    memcpy(pPixels, a_other.pPixels, sizeof(RGBA) * (size_t)width * height);
  }

  void TextureData::Clear()
  {
    flags.SetData(0);
    width = 0;
    height = 0;
    delete[] pPixels;
    pPixels = nullptr;
  }

  size_t TextureData::Size() const
  {
    size_t result = 0;
    result += Core::SerializedSize(flags.GetData());
    result += Core::SerializedSize(width);
    result += Core::SerializedSize(height);
    result += (sizeof(RGBA::DataType) * width * height);
    return result;
  }
}