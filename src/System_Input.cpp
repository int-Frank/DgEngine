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

namespace DgE
{
  MAKE_SYSTEM_DEFINITION(System_Input)

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

  void System_Input::AddBinding(InputCode a_code, InputEvent a_event, InputMessageTranslator a_callback, void *a_pData)
  {
    m_bindings[GET_FULL_INPUT_CODE(a_code, a_event)] = {a_callback, a_pData};
  }

  void System_Input::Update(float a_dt)
  {
    while (true)
    {
      TRef<Message> pMsg = m_pEventPoller->NextEvent();
      if (pMsg.Get() == nullptr)
        break;

      // Document InputCodes mapping to message types
      if (pMsg->GetID() == Message_Input_Key::GetStaticID())
      {
        TRef<Message_Input_Key> pTemp = StaticPointerCast<Message_Input_Key, Message>(pMsg);
        uint32_t code = GET_FULL_INPUT_CODE(pTemp->code, pTemp->event);
        auto it = m_bindings.find(code);
        if (it != m_bindings.end())
          it->second.func(pTemp.Get(), it->second.pData);
      }
      else if (pMsg->GetID() == Message_Input_Text::GetStaticID())
      {
        TRef<Message_Input_Text> pTemp = StaticPointerCast<Message_Input_Text, Message>(pMsg);
        uint32_t code = GET_FULL_INPUT_CODE(pTemp->code, pTemp->event);
        auto it = m_bindings.find(code);
        if (it != m_bindings.end())
          it->second.func(pTemp.Get(), it->second.pData);
      }
      else if (pMsg->GetID() == Message_Input_Mouse::GetStaticID())
      {
        TRef<Message_Input_Mouse> pTemp = StaticPointerCast<Message_Input_Mouse, Message>(pMsg);
        uint32_t code = GET_FULL_INPUT_CODE(pTemp->code, pTemp->event);
        auto it = m_bindings.find(code);
        if (it != m_bindings.end())
          it->second.func(pTemp.Get(), it->second.pData);
      }
      else if (pMsg->GetCategory() != MC_Input) // Pass on everything but raw input
      {
        POST(pMsg);
      }
      else
      {
        DG_ASSERT("Unhandled Input Message!");
      }
    }
  }
}
