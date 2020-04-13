//@group Renderer/RenderThread

#include "RenderThreadData.h"
#include "core_Assert.h"

namespace Engine
{
  RenderThreadData* RenderThreadData::s_instance = nullptr;

  bool RenderThreadData::Init()
  {
    BSR_ASSERT(s_instance == nullptr, "RenderThreadData already intialised!");
    s_instance = new RenderThreadData();
    return true;
  }

  void RenderThreadData::ShutDown()
  {
    delete s_instance;
    s_instance = nullptr;
  }

  RenderThreadData* RenderThreadData::Instance()
  {
    return s_instance;
  }
}