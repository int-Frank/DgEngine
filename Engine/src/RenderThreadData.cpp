//@group Renderer/RenderThread

#include "RenderThreadData.h"
#include "BSR_Assert.h"

namespace Engine
{
  RenderThreadData* RenderThreadData::s_instance = nullptr;

  RenderThreadData::~RenderThreadData()
  {
    for (auto kv : VAOs)  delete kv.second;
    for (auto kv : IBOs)  delete kv.second;
    for (auto kv : VBOs)  delete kv.second;
    for (auto kv : UBOs)  delete kv.second;
    //for (auto kv : SSBOs)  delete kv.second;
    for (auto kv : bindingPoints)  delete kv.second;
    for (auto kv : textures)  delete kv.second;
    for (auto kv : rendererPrograms)  delete kv.second;
  }

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