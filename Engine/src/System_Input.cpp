//@group Systems

#include <cstring>

#include "System_Input.h"
#include "Framework.h"
#include "core_Log.h"
#include "InputCodes.h"
#include "EngineMessages.h"
#include "MessageBus.h"

#include "IEventPoller.h"
#include "IMouseController.h"

namespace Engine
{
  //------------------------------------------------------------------------------------
  // System_Input
  //------------------------------------------------------------------------------------

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

  void System_Input::AddBinding(uint32_t a_inputMessageID, InputMessageTranslator a_callback)
  {
    m_bindings[a_inputMessageID] = a_callback;
  }

  void System_Input::Update(float a_dt)
  {
    while (true)
    {
      TRef<Message> pMsg = m_pEventPoller->NextEvent();
      if (pMsg.Get() == nullptr)
        break;

      auto it = m_bindings.find(pMsg->GetID());
      if (it != m_bindings.end())
        it->second(pMsg.Get());
      else if (pMsg->GetCategory() != MC_Input) // Pass on everything but raw input
        POST(pMsg);
    }
  }
}
