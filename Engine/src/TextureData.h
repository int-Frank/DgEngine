//@group Renderer

#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <stdint.h>
#include "Utils.h"

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

  enum class TexturePixelType
  {
    R8,
    RG8,
    RGB8,
    RGBA8,
  };

  size_t GetPixelSize(TexturePixelType);

  class TextureAttributes
  {
  public:

    TextureAttributes();

    TextureWrap GetWrap() const;
    TextureFilter GetFilter() const;
    TextureMipmapFilter GetMipmapFilter() const;
    TexturePixelType GetPixelType() const;
    bool IsMipmapped() const;

    void SetPixelType(TexturePixelType);
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
    ~TextureData();

    //Takes ownership of a_pixels
    TextureData(uint32_t a_width, uint32_t a_height, void * a_pixels, TextureAttributes a_attrs);

    TextureData(TextureData const &);
    TextureData & operator=(TextureData const &);

    TextureData(TextureData &&) noexcept;
    TextureData & operator=(TextureData &&) noexcept;

    //Takes ownership of a_pixels
    void Set(uint32_t a_width, uint32_t a_height, void * a_pixels, TextureAttributes a_attrs);

    void Duplicate(TextureData const &);
    size_t Size() const;
    void* Serialize(void*);

    //Will not allocate pixel data, but just point to it
    void const * Deserialize(void const*);
    void Clear();

    TextureAttributes attrs;
    uint32_t          width;
    uint32_t          height;
    uint8_t *         pPixels;
  };
}

#endif