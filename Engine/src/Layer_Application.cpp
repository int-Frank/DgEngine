//@group Layers

#include "Layer_Application.h"
#include "EngineMessages.h"

namespace Engine
{
  Layer_Application::Layer_Application()
  {
  
  }

  Layer_Application::~Layer_Application()
  {

  }

  void Layer_Application::OnAttach()
  {

  }

  void Layer_Application::OnDetach()
  {

  }

  void Layer_Application::Update(float a_dt)
  {

  }

  void Layer_Application::HandleMessage(Message* a_pMsg)
  {
    DISPATCH_MESSAGE(Message_Command);
  }

  void Layer_Application::HandleMessage(Message_Command* a_pMsg)
  {
    a_pMsg->Run();
    a_pMsg->SetFlag(Message::Flag::Handled);
  }
}