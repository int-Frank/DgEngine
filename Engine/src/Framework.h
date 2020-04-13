//@group Framework

#ifndef EN_FRAMEWORK_H
#define EN_FRAMEWORK_H

#include <string>

#include "core_ErrorCodes.h"

#include "IWindow.h"
#include "IEventPoller.h"
#include "IMouseController.h"
#include "IGraphicsContext.h"
#include "Buffer.h"
#include "Memory.h"

namespace Engine
{
  class Framework
  {
  public:

    struct ImGui_InitData
    {
      int window_w;
      int window_h;
    };

  public:

    Framework();
    ~Framework();

    static Framework * Instance();
    static Core::ErrorCode Init();
    static Core::ErrorCode ShutDown();

    //There can only be one of these objects...
    Ref<IWindow>          GetWindow();
    Ref<IEventPoller>     GetEventPoller();
    Ref<IMouseController> GetMouseController();
    Ref<IGraphicsContext> GetGraphicsContext();

    //Audio
    //IAudio *       GetAudio();

  private:

    //These are implemented in the relevent cpp files
    void InitWindow();
    void InitEventPoller();
    void InitMouseController();
    void InitGraphicsContext();

  public:

    void SetWindow(IWindow *);
    void SetEventPoller(IEventPoller *);
    void SetMouseController(IMouseController*);
    void SetGraphicsContext(IGraphicsContext*);

  private:

    static Framework * s_instance;

    class PIMPL;
    PIMPL * m_pimpl;

  };
}


#endif