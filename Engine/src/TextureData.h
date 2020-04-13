//@group Renderer

#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <stdint.h>
#include "core_utils.h"

namespace Engine
{

  enum class TextureWrap
  {
    None,
    Clamp,
    Repeat,
    Mirror
  };

  enum class TextureFilter
  {
    Nearest,
    Linear
  };

  enum class TextureMipmapFilter
  {
    Nearest_Nearest,
    Linear_Nearest,
    Nearest_Linear,
    Linear_Linear
  };

  class TextureFlags
  {
  public:

    TextureFlags();

    TextureWrap GetWrap() const;
    TextureFilter GetFilter() const;
    TextureMipmapFilter GetMipmapFilter() const;
    bool IsMipmapped() const;

    void SetWrap(TextureWrap);
    void SetFilter(TextureFilter);
    void SetMipmapFilter(TextureMipmapFilter);
    void SetIsMipmapped(bool);

    void SetData(uint32_t);
    uint32_t GetData() const;

  private:

    uint32_t m_data;
  };

  class TextureData
  {
  public:

    TextureData();

    //Takes ownership of a_pixels
    TextureData(uint32_t a_width, uint32_t a_height, RGBA * a_pixels, TextureFlags a_flags);

    TextureData(TextureData const &);
    TextureData & operator=(TextureData const &);

    TextureData(TextureData &&) noexcept;
    TextureData & operator=(TextureData &&) noexcept;

    //Takes ownership of a_pixels
    void Set(uint32_t a_width, uint32_t a_height, RGBA * a_pixels, TextureFlags a_flags);

    void Duplicate(TextureData const &);
    size_t Size() const;
    void* Serialize(void*);

    //Will not allocate pixel data, but just point to it
    void const * Deserialize(void const*);
    void Clear();

    TextureFlags  flags;
    uint32_t      width;
    uint32_t      height;
    RGBA *        pPixels;
  };
}

#endif