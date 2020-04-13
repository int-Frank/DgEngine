//@group Interface

#ifndef EN_IGRAPHICSCONTEXT_H
#define EN_IGRAPHICSCONTEXT_H

#include "core_ErrorCodes.h"

namespace Engine
{
  class IGraphicsContext
  {
  public:

    virtual ~IGraphicsContext(){};

    virtual Core::ErrorCode Init() = 0;
    virtual Core::ErrorCode ShutDown() = 0;
    virtual void SwapBuffers() = 0;

  private:

  };
}

#endif