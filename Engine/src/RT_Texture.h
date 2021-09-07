//@group Renderer/RenderThread

#ifndef RT_TEXTURE_H
#define RT_TEXTURE_H

#include "RT_RendererAPI.h"
#include "TextureData.h"

namespace DgE
{
  class RT_Texture2D
  {
    RT_Texture2D(TextureData const & a_data);
  public:

    ~RT_Texture2D();

    static RT_Texture2D * Create(TextureData const &);

    void Bind(uint32_t slot = 0);

  private:
    RendererID    m_rendererID;
    TextureAttributes  m_attrs;
  };
}

#endif