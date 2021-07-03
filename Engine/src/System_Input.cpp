//@group Systems

#include <cstring>

#include "System_Input.h"
#include "Framework.h"
#include "Log.h"
#include "InputCodes.h"
#include "EngineMessages.h"
#include "MessageBus.h"

#include "IEventPoller.h"
#include "IMouseController.h"

namespace Engine
{
  System_Input::System_Input()
    : m_pEventPoller(Framework::Instance()->GetEventPoller())
  {

  }

  System_Input::~System_Input()
  {

  }

  void System_Input::HandleMessage(Message *)
  {
  
  }

  void System_Input::ClearBindings()
  {
    m_bindings.clear();
  }

  void System_Input::AddBinding(InputCode a_code, InputEvent a_event, InputMessageTranslator a_callback)
  {
    m_bindings[((uint32_t(a_code) << 16) | uint32_t(a_event)] = a_callback;
  }

  void System_Input::Update(float a_dt)
  {
    while (true)
    {
      TRef<Message> pMsg = m_pEventPoller->NextEvent();
      if (pMsg.Get() == nullptr)
        break;

      if (pMsg->GetID() == Message_Input::GetStaticID())
      {
        TRef<Message_Input> pTemp = StaticPointerCast<Message_Input, Message>(pMsg);
        uint32_t code = GET_FULL_INPUT_CODE(pTemp->code, pTemp->event);
        auto it = m_bindings.find(code);
        if (it != m_bindings.end())
          it->second(pTemp.Get());
      }
      else if (pMsg->GetCategory() != MC_Input) // Pass on everything but raw input
      {
        POST(pMsg);
      }
    }
  }
}
