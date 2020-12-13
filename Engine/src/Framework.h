//@group Framework

#ifndef EN_FRAMEWORK_H
#define EN_FRAMEWORK_H

#include <string>

#include "DgError.h"

#include "IWindow.h"
#include "IEventPoller.h"
#include "IMouseController.h"
#include "IGraphicsContext.h"
#include "IFontAtlas.h"
#include "Buffer.h"

#undef ITEM
#define ITEM(x) public: I ## x * Get ## x(); void Set ## x(I ## x *); private: Dg::ErrorCode Init ## x();

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

    Framework();
    ~Framework();

    static Framework * Instance();
    static Dg::ErrorCode Init();
    static Dg::ErrorCode ShutDown();

    UNROLL_FRAMEWORK_CLASSES

  private:

    static Framework * s_instance;

    class PIMPL;
    PIMPL * m_pimpl;

  };
}


#endif