//@group Renderer/RenderThread

#include <glad/glad.h>
#include "RT_Texture.h"
#include "RT_RendererAPI.h"
#include "BSR_Assert.h"

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

  RT_Texture2D::RT_Texture2D(TextureData const & a_data)
    : m_rendererID(0)
    , m_attrs(a_data.attrs)
  {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGL(m_attrs.GetWrap()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGL(m_attrs.GetWrap()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGL(m_attrs.GetFilter()));
    glTextureParameterf(m_rendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().maxAnisotropy);

    if (m_attrs.IsMipmapped())
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGL(m_attrs.GetMipmapFilter()));
    else
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGL(m_attrs.GetFilter()));

    switch (m_attrs.GetPixelType())
    {
      case TexturePixelType::R8:
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, a_data.width, a_data.height, 0, GL_RED, GL_UNSIGNED_BYTE, a_data.pPixels);
        break;
      }
      case TexturePixelType::RG8:
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, a_data.width, a_data.height, 0, GL_RG, GL_UNSIGNED_BYTE, a_data.pPixels);
        break;
      }
      case TexturePixelType::RGB8:
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_data.width, a_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, a_data.pPixels);
        break;
      }
      case TexturePixelType::RGBA8:
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_data.width, a_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_data.pPixels);
        break;
      }
      default:
      {
        BSR_ASSERT(false, "Pixel type not yet implemented!");
      }
    }

    if (m_attrs.IsMipmapped())
      glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  RT_Texture2D::~RT_Texture2D()
  {
    GLuint count = 1;
    glDeleteTextures(m_rendererID, &count);
    m_rendererID = 0;
  }

  RT_Texture2D * RT_Texture2D::Create(TextureData const & a_data)
  {
    return new RT_Texture2D(a_data);
  }

  void RT_Texture2D::Bind(uint32_t a_slot)
  {
    glBindTextureUnit(a_slot, m_rendererID);
  }
}