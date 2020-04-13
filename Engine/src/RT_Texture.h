//@group Renderer/RenderThread

#ifndef RT_TEXTURE_H
#define RT_TEXTURE_H

#include "RT_RendererAPI.h"
#include "TextureData.h"

namespace Engine
{
  class RT_Texture2D
  {
  public:

    RT_Texture2D();
    ~RT_Texture2D();

    void Init(TextureData const &);
    void Destroy();

    void Bind(uint32_t slot = 0);

  private:
    RendererID    m_rendererID;
    TextureFlags  m_flags;
  };
}

#endif