//@group Renderer

#ifndef RENDERRESOURCE_H
#define RENDERRESOURCE_H

#include <stdint.h>

namespace DgE
{
  typedef uint32_t RenderResourceID;
#define INVALID_RENDER_RESOURE_ID 0xFFFFFFFF

  class RenderResource
  {
  public:

    RenderResource();
    virtual ~RenderResource();

    RenderResourceID GetID() const;

  protected:

    RenderResourceID const m_id;

  private:
    static RenderResourceID s_nextID;
  };
}

#endif