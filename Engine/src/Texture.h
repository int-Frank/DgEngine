//@group Renderer

/*
  Original Copyright Yan Chernikov <github.com/TheCherno/Hazel-dev> and contributors.

  The following code is a derivative work of the code from the GitHub project 'Hazel-dev',
  which is licensed under:

                                  Apache License
                             Version 2.0, January 2004
                          http://www.apache.org/licenses

  This code therefore is also licensed under the terms
  of the Apache License Version 2.0

  Copyright 2017-2019 Frank Hart <frankhart010@gmail.com>
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "TextureData.h"
#include "Utils.h"
#include "RenderResource.h"
#include "Memory.h"

namespace DgE
{
  enum class ResizeMethod
  {
    HQx,
    BRz
  };

  class Texture2D : public RenderResource
  {
    Texture2D();
  public:

    static Ref<Texture2D> Create();

    ~Texture2D();

    //Once we are done loading/manipulating the texture, we need to upload it to the video card.
    void Upload(bool freePixels = false);
    void Clear();
    void Bind(uint32_t slot = 0) const;

    void Set(uint32_t width, uint32_t height, void * pPixels, TextureAttributes attrs);

    //Loading...
    //bool LoadFromRawData(uint32_t width, uint32_t height, TextureWrap wrap, Colour* pixels, TextureFlags flags);
    //bool LoadFromDataFile(void const*);
    //bool LoadFromImageFile(std::string const & path);
    //bool LoadFromText(TextData const &);

    //uint32_t GetWidth() const;
    //uint32_t GetHeight() const;

    //void Resize(uint32_t width, uint32_t height);
    //void Resize(ResizeMethod, int factor);

    //Colour& GetPixel(uint32_t width, uint32_t height);
    //void SetPixel(uint32_t width, uint32_t height, Colour value);

    //void Set(uint32_t x, uint32_t y,
    //         uint32_t width, uint32_t height,
    //         byte const * buffer);

  private:

    TextureData m_data;
  };
}

#endif