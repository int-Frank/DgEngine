//@group Framework

#include "SDL.h"
#include "Options.h"
#include "core_Log.h"
#include "Framework.h"
#include "InputCodes.h"
#include "core_Assert.h"

namespace Engine
{
  static char*        g_ClipboardTextData = nullptr;

  class Framework::PIMPL
  {
  public:

    PIMPL()
      : window(nullptr)
      , eventPoller(nullptr)
      , mouseController(nullptr)
      , graphicsContext(nullptr)
    {

    }

    ~PIMPL()
    {

    }

    Ref<IWindow>          window;
    Ref<IEventPoller>     eventPoller;
    Ref<IMouseController> mouseController;
    Ref<IGraphicsContext> graphicsContext;
  };

  Framework * Framework::s_instance(nullptr);

  Framework::Framework()
    : m_pimpl(new PIMPL())
  {

  }

  Framework::~Framework()
  {
    delete m_pimpl;
  }

  Framework * Framework::Instance()
  {
    BSR_ASSERT(s_instance != nullptr, "Framework not initialized!");
    return s_instance;
  }

  Core::ErrorCode Framework::Init()
  {
    BSR_ASSERT(s_instance == nullptr, "Framework already initialized!");
    s_instance = new Framework();
    Core::ErrorCode result = Core::EC_None;
    do
    {
      //-----------------------------------------------------------------------------------------
      //Init APIs...
      //-----------------------------------------------------------------------------------------

      //Init SDL...
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
      {
        LOG_ERROR("Unable to initialize SDL: {}", SDL_GetError());
        result = Core::EC_Error;
        break;
      }

      //-----------------------------------------------------------------------------------------
      //Init Modules...
      //-----------------------------------------------------------------------------------------
      s_instance->InitWindow(); //Init window, create OpenGL context, init GLAD
      s_instance->InitEventPoller();
      s_instance->InitMouseController();
      s_instance->InitGraphicsContext();

    } while (false);
    return result;
  }

  Core::ErrorCode Framework::ShutDown()
  {
    Core::ErrorCode result = Core::EC_None;

    delete s_instance;
    s_instance = nullptr;

    if (g_ClipboardTextData)
      SDL_free(g_ClipboardTextData);
    g_ClipboardTextData = nullptr;

    SDL_Quit();

    return result;
  }

  Ref<IWindow> Framework::GetWindow()
  {
    return m_pimpl->window;
  }

  Ref<IEventPoller> Framework::GetEventPoller()
  {
    return m_pimpl->eventPoller;
  }

  Ref<IMouseController> Framework::GetMouseController()
  {
    return m_pimpl->mouseController;
  }

  Ref<IGraphicsContext> Framework::GetGraphicsContext()
  {
    return m_pimpl->graphicsContext;
  }

  void Framework::SetWindow(IWindow * a_window)
  {
    BSR_ASSERT(m_pimpl->window.IsNull(), "Window already exists!");
    m_pimpl->window = Ref<IWindow>(a_window);
  }

  void Framework::SetEventPoller(IEventPoller * a_ep)
  {
    BSR_ASSERT(m_pimpl->eventPoller.IsNull(), "EventPoller already exists!");
    m_pimpl->eventPoller = Ref<IEventPoller>(a_ep);
  }

  void Framework::SetMouseController(IMouseController* a_mc)
  {
    BSR_ASSERT(m_pimpl->mouseController.IsNull(), "MouseController already exists!");
    m_pimpl->mouseController = Ref<IMouseController>(a_mc);
  }

  void Framework::SetGraphicsContext(IGraphicsContext* a_gc)
  {
    BSR_ASSERT(m_pimpl->graphicsContext.IsNull(), "GraphicsContext already exists!");
    m_pimpl->graphicsContext = Ref<IGraphicsContext>(a_gc);
  }
}
