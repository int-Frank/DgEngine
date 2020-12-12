//@group Framework

#include "SDL.h"
#include "Options.h"
#include "Log.h"
#include "Framework.h"
#include "InputCodes.h"
#include "BSR_Assert.h"

namespace Engine
{
  static char*        g_ClipboardTextData = nullptr;

  class Framework::PIMPL
  {
  public:

    PIMPL()
      : pWindow(nullptr)
      , pEventPoller(nullptr)
      , pMouseController(nullptr)
      , pGraphicsContext(nullptr)
      , pFontAtlas(nullptr)
    {

    }

    ~PIMPL()
    {
      delete pFontAtlas;
      delete pEventPoller;
      delete pMouseController;
      delete pGraphicsContext;
      delete pWindow;
    }

    IWindow *          pWindow;
    IEventPoller *     pEventPoller;
    IMouseController * pMouseController;
    IGraphicsContext * pGraphicsContext;
    IFontAtlas *       pFontAtlas;
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

  ErrorCode Framework::Init()
  {
    BSR_ASSERT(s_instance == nullptr, "Framework already initialized!");
    s_instance = new Framework();
    ErrorCode result = EC_None;
    do
    {
      //-----------------------------------------------------------------------------------------
      //Init APIs...
      //-----------------------------------------------------------------------------------------

      //Init SDL...
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
      {
        LOG_ERROR("Unable to initialize SDL: {}", SDL_GetError());
        result = EC_Error;
        break;
      }

      //-----------------------------------------------------------------------------------------
      //Init Modules...
      //-----------------------------------------------------------------------------------------
      s_instance->InitWindow(); //Init window, create OpenGL context, init GLAD
      s_instance->InitEventPoller();
      s_instance->InitMouseController();
      s_instance->InitGraphicsContext();
      s_instance->InitFontAtlas();

    } while (false);
    return result;
  }

  ErrorCode Framework::ShutDown()
  {
    ErrorCode result = EC_None;

    delete s_instance;
    s_instance = nullptr;

    if (g_ClipboardTextData)
      SDL_free(g_ClipboardTextData);
    g_ClipboardTextData = nullptr;

    SDL_Quit();

    return result;
  }

  IWindow * Framework::GetWindow()
  {
    return m_pimpl->pWindow;
  }

  IEventPoller * Framework::GetEventPoller()
  {
    return m_pimpl->pEventPoller;
  }

  IMouseController * Framework::GetMouseController()
  {
    return m_pimpl->pMouseController;
  }

  IGraphicsContext * Framework::GetGraphicsContext()
  {
    return m_pimpl->pGraphicsContext;
  }

  IFontAtlas * Framework::GetFontAtlas()
  {
    return m_pimpl->pFontAtlas;
  }

  void Framework::SetWindow(IWindow * a_ptr)
  {
    BSR_ASSERT(m_pimpl->pWindow == nullptr, "Window already exists!");
    m_pimpl->pWindow = a_ptr;
  }

  void Framework::SetEventPoller(IEventPoller * a_ptr)
  {
    BSR_ASSERT(m_pimpl->pEventPoller == nullptr, "EventPoller already exists!");
    m_pimpl->pEventPoller = a_ptr;
  }

  void Framework::SetMouseController(IMouseController* a_ptr)
  {
    BSR_ASSERT(m_pimpl->pMouseController == nullptr, "MouseController already exists!");
    m_pimpl->pMouseController = a_ptr;
  }

  void Framework::SetGraphicsContext(IGraphicsContext * a_ptr)
  {
    BSR_ASSERT(m_pimpl->pGraphicsContext == nullptr, "GraphicsContext already exists!");
    m_pimpl->pGraphicsContext = a_ptr;
  }

  void Framework::SetFontAtlas(IFontAtlas * a_ptr)
  {
    BSR_ASSERT(m_pimpl->pFontAtlas == nullptr, "FontAtlas already exists!");
    m_pimpl->pFontAtlas = a_ptr;
  }
}
