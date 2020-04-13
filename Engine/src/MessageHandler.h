//@group Messages

#ifndef EN_MESSAGEHANDLER_H
#define EN_MESSAGEHANDLER_H

#include "EngineMessages.h"

#define DISPATCH_MESSAGE(TYPE) do{if (a_pMsg->GetID() == TYPE::GetStaticID()) {HandleMessage(dynamic_cast<TYPE*>(a_pMsg)); return;}} while (false)

namespace Engine
{
  class MessageHandler
  {
    
  public:

    virtual ~MessageHandler() = default;
    virtual void HandleMessage(Message *) {}
  };
}

#endif