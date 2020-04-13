//@group Interface

#ifndef EN_IEVENTPOLLER_H
#define EN_IEVENTPOLLER_H

#include "Memory.h"
#include "Message.h"

namespace Engine
{
  class Message;

  class IEventPoller
  {
  public:

    virtual ~IEventPoller(){}

    //Returns nullptr if no new event
    virtual TRef<Message> NextEvent() =0;

  };
}

#endif