//@group Interface

#ifndef EN_IGRAPHICSCONTEXT_H
#define EN_IGRAPHICSCONTEXT_H

#include "DgError.h"

namespace DgE
{
  class IGraphicsContext
  {
  public:

    virtual ~IGraphicsContext(){};

    virtual Dg::ErrorCode Init() = 0;
    virtual Dg::ErrorCode ShutDown() = 0;
    virtual void SwapBuffers() = 0;
    virtual void Resize(uint32_t w, uint32_t h) = 0;

  private:

  };
}

#endif