//@group Systems

#include "System_Application.h"
#include "EngineMessages.h"

namespace Engine
{
  System_Application::System_Application()
  {
  
  }

  System_Application::~System_Application()
  {

  }

  void System_Application::OnAttach()
  {

  }

  void System_Application::OnDetach()
  {

  }

  void System_Application::Update(float a_dt)
  {

  }

  void System_Application::HandleMessage(Message* a_pMsg)
  {
    DISPATCH_MESSAGE(Message_Command);
  }

  void System_Application::HandleMessage(Message_Command* a_pMsg)
  {
    a_pMsg->Run();
    a_pMsg->SetFlag(Message::Flag::Handled, true);
  }
}