//@group Framework

#include "SDL.h"
#include "Options.h"
#include "Log.h"
#include "Framework.h"
#include "InputCodes.h"
#include "Options.h"

namespace DgE
{
  static char* g_ClipboardTextData = nullptr;

  class Framework::PIMPL
  {
  public:

    PIMPL()
      : DUMMY(0)
#undef ITEM
#define ITEM(m) , p##m(nullptr)
      UNROLL_FRAMEWORK_CLASSES
    {

    }

    ~PIMPL()
    {
#undef ITEM
#define ITEM(m) delete p##m;
      UNROLL_FRAMEWORK_CLASSES
    }

    uint32_t           DUMMY;

#undef ITEM
#define ITEM(m) I##m * p##m;
    UNROLL_FRAMEWORK_CLASSES
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
    DG_ASSERT(s_instance != nullptr, "Framework not initialized!");
    return s_instance;
  }

  Dg::ErrorCode Framework::Init()
  {
    DG_ASSERT(s_instance == nullptr, "Framework already initialized!");
    s_instance = new Framework();
    Dg::ErrorCode result = Dg::ErrorCode::None;
    do
    {
      //-----------------------------------------------------------------------------------------
      //Init APIs...
      //-----------------------------------------------------------------------------------------

      //Init SDL...
      if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
      {
        LOG_ERROR("Unable to initialize SDL: {}", SDL_GetError());
        result = Dg::ErrorCode::Failure;
        break;
      }

      //-----------------------------------------------------------------------------------------
      //Init Modules...
      //-----------------------------------------------------------------------------------------

#undef ITEM
#define ITEM(m) result = s_instance->Init##m(); if (result != Dg::ErrorCode::None) { LOG_ERROR("Failed to Initialise module: " #m); break; }
      UNROLL_FRAMEWORK_CLASSES

    } while (false);
    return result;
  }

  Dg::ErrorCode Framework::ShutDown()
  {
    Dg::ErrorCode result = Dg::ErrorCode::None;

    delete s_instance;
    s_instance = nullptr;

    if (g_ClipboardTextData)
      SDL_free(g_ClipboardTextData);
    g_ClipboardTextData = nullptr;

    SDL_Quit();

    return result;
  }

#undef ITEM
#define ITEM(m) I##m * Framework::Get##m() { return m_pimpl->p##m;}
  UNROLL_FRAMEWORK_CLASSES

#undef ITEM
#define ITEM(m) void Framework::Set##m(I##m * a_ptr) { DG_ASSERT(m_pimpl->p##m == nullptr, #m " already exists!"); m_pimpl->p##m = a_ptr;}
  UNROLL_FRAMEWORK_CLASSES
}
