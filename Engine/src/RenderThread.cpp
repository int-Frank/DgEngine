//@group Renderer/RenderThread

#include "BSR_Assert.h"
#include "RenderThread.h"
#include "Framework.h"
#include "RT_RendererAPI.h"
#include "RenderThreadData.h"
#include "RT_BindingPoint.h"
#include "Renderer.h"

namespace Engine
{
  //-----------------------------------------------------------------------------------------------
  // Render Thread worker function
  //-----------------------------------------------------------------------------------------------
  static void RenderThreadWorker()
  {
    if (Framework::Instance()->GetGraphicsContext()->Init() != ErrorCode::EC_None)
    {
      LOG_ERROR("Unable to set rendering context!");
      RenderThread::Instance()->RenderThreadInitFailed();
      return;
    }

    RendererAPI::Init();
    RT_BindingPoint::Init();
    RenderThreadData::Init();
    RenderThread::Instance()->RenderThreadInitFinished();

    while (!RenderThread::Instance()->ShouldExit())
    {
      Renderer::Instance()->ExecuteRenderCommands();
      RenderThread::Instance()->RenderThreadFrameFinished();
    }
    RenderThreadData::ShutDown();
    RendererAPI::ShutDown();

    if (Framework::Instance()->GetGraphicsContext()->ShutDown() != ErrorCode::EC_None)
      LOG_ERROR("Trouble shutting down the rendering context!!");

    RenderThread::Instance()->RenderThreadShutDownFinished();
  }

  //-----------------------------------------------------------------------------------------------
  // Render Thread
  //-----------------------------------------------------------------------------------------------
  RenderThread * RenderThread::s_instance = nullptr;

  RenderThread::RenderThread()
    : m_renderDone(false)
    , m_returnCode(ReturnCode::None)
    , m_shouldStop(false)
  {

  }

  bool RenderThread::Init()
  {
    BSR_ASSERT(s_instance == nullptr, "Trying to initialise twice!");
    s_instance = new RenderThread();
    return s_instance->Start();
  }

  void RenderThread::ShutDown()
  {
    s_instance->Stop();
    delete s_instance;
    s_instance = nullptr;
  }

  RenderThread* RenderThread::Instance()
  {
    BSR_ASSERT(s_instance != nullptr, "RenderThread not initialised!");
    return s_instance;
  }

  bool RenderThread::Start()
  {
    m_renderDone = false;
    m_renderThread = std::thread(RenderThreadWorker);

    while (!m_renderDone)
      std::this_thread::yield();

    return m_returnCode == ReturnCode::Ready;
  }

  void RenderThread::Stop()
  {
    while (!m_renderDone)
      std::this_thread::yield();

    m_shouldStop = true;
    m_renderDone = false;

    m_renderThread.join();
  }

  void RenderThread::Sync()
  {
    while (!m_renderDone)
      std::this_thread::yield();
  }

  void RenderThread::Continue()
  {
    m_renderDone = false;
  }

  void RenderThread::RenderThreadInitFinished()
  {
    m_returnCode = ReturnCode::Ready;
    m_renderDone = true;

    while (m_renderDone)
      std::this_thread::yield();
  }

  void RenderThread::RenderThreadInitFailed()
  {
    m_returnCode = ReturnCode::Fail;
    m_renderDone = true;
  }

  void RenderThread::RenderThreadShutDownFinished()
  {

  }

  void RenderThread::RenderThreadFrameFinished()
  {
    m_renderDone = true;
    while (m_renderDone)
      std::this_thread::yield();
  }

  bool RenderThread::ShouldExit()
  {
    return m_shouldStop;
  }
}