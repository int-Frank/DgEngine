//@group Renderer

#include "RenderResource.h"

namespace DgE
{
  RenderResourceID RenderResource::s_nextID = 0;

  RenderResource::RenderResource()
    : m_id(++s_nextID)
  {
  
  }

  RenderResource::~RenderResource()
  {

  }

  RenderResourceID RenderResource::GetID() const
  {
    return m_id;
  }
}