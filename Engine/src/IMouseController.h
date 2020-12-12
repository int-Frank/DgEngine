//@group Interface

#ifndef EN_IMOUSECONTROLLER_H
#define EN_IMOUSECONTROLLER_H

#include "ErrorCodes.h"

namespace Engine
{
  class IMouseController
  {
  public:

    ~IMouseController(){}
    virtual ErrorCode Grab() =0;
    virtual ErrorCode Release() =0;

    virtual void GetPos(int & x, int & y) =0;
    virtual void MoveToPos(int x, int y) =0;

  private:

  };
}

#endif