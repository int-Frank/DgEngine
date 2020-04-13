//@group Renderer/RenderThread

#include <glad/glad.h>
#include "RT_Texture.h"
#include "RT_RendererAPI.h"

namespace Engine
{
  static GLenum GetGL(TextureWrap a_val)
  {
    switch (a_val)
    {
      case TextureWrap::Mirror: return GL_MIRRORED_REPEAT;
      case TextureWrap::Repeat: return GL_REPEAT;
      case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
      default:                  return GL_CLAMP_TO_BORDER;
    }
  }

  static GLenum GetGL(TextureFilter a_val)
  {
    switch (a_val)
    {
      case TextureFilter::Linear: return GL_LINEAR;
      default:                    return GL_NEAREST;
    }
  }

  static GLenum GetGL(TextureMipmapFilter a_val)
  {
    switch (a_val)
    {
      case TextureMipmapFilter::Linear_Linear:  return GL_LINEAR_MIPMAP_LINEAR;
      case TextureMipmapFilter::Linear_Nearest: return GL_LINEAR_MIPMAP_NEAREST;
      case TextureMipmapFilter::Nearest_Linear: return GL_NEAREST_MIPMAP_LINEAR;
      default:                                  return GL_NEAREST_MIPMAP_NEAREST;
    }
  }

  RT_Texture2D::RT_Texture2D()
    : m_rendererID(0)
  {

  }

  RT_Texture2D::~RT_Texture2D()
  {

  }

  void RT_Texture2D::Init(TextureData const & a_data)
  {
    m_flags = a_data.flags;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGL(m_flags.GetWrap()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGL(m_flags.GetWrap()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGL(m_flags.GetFilter()));
    glTextureParameterf(m_rendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().maxAnisotropy);

    if (m_flags.IsMipmapped())
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGL(m_flags.GetMipmapFilter()));
    else
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGL(m_flags.GetFilter()));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_data.width, a_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)a_data.pPixels);
    
    if (m_flags.IsMipmapped())
      glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RT_Texture2D::Destroy()
  {
    GLuint count = 1;
    glDeleteTextures(m_rendererID, &count);
    m_rendererID = 0;
  }

  void RT_Texture2D::Bind(uint32_t a_slot)
  {
    glBindTextureUnit(a_slot, m_rendererID);
  }
}