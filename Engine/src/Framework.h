//@group Framework

#ifndef EN_FRAMEWORK_H
#define EN_FRAMEWORK_H

#include <string>

#include "core_ErrorCodes.h"

#include "IWindow.h"
#include "IEventPoller.h"
#include "IMouseController.h"
#include "IGraphicsContext.h"
#include "IFontAtlas.h"
#include "Buffer.h"

#undef ITEM
#define ITEM(x) public: I ## x * Get ## x(); void Set ## x(I ## x *); private: void Init ## x();

#define UNROLL_FRAMEWORK_CLASSES \
 ITEM(Window)\
 ITEM(EventPoller)\
 ITEM(MouseController)\
 ITEM(GraphicsContext)\
 ITEM(FontAtlas)

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

    UNROLL_FRAMEWORK_CLASSES

  private:

    static Framework * s_instance;

    class PIMPL;
    PIMPL * m_pimpl;

  };
}


#endif