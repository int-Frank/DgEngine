//@group Systems

#include "Memory.h"

#include "System_Window.h"
#include "EngineMessages.h"
#include "Application.h"
#include "IWindow.h"

namespace Engine
{
  System_Window::System_Window(IWindow * a_pWindow)
    : m_pWindow(a_pWindow)
  {

  }

  System_Window::~System_Window()
  {

  }

  void System_Window::OnAttach()
  {

  }

  void System_Window::OnDetach()
  {

  }

  void System_Window::Update(float a_dt)
  {
    m_pWindow->Update();
  }

  void System_Window::HandleMessage(Message* a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_Window)
      return;

    DISPATCH_MESSAGE(Message_Window_Shown);
    DISPATCH_MESSAGE(Message_Window_Hidden);
    DISPATCH_MESSAGE(Message_Window_Exposed);
    DISPATCH_MESSAGE(Message_Window_Moved);
    DISPATCH_MESSAGE(Message_Window_Resized);
    DISPATCH_MESSAGE(Message_Window_Minimized);
    DISPATCH_MESSAGE(Message_Window_Maximized);
    DISPATCH_MESSAGE(Message_Window_Restored);
    DISPATCH_MESSAGE(Message_Window_Enter);
    DISPATCH_MESSAGE(Message_Window_Leave);
    DISPATCH_MESSAGE(Message_Window_Focus_Gained);
    DISPATCH_MESSAGE(Message_Window_Focus_Lost);
    DISPATCH_MESSAGE(Message_Window_Close);
    DISPATCH_MESSAGE(Message_Window_Take_Focus);
  }

  void System_Window::HandleMessage(Message_Window_Shown*)
  {

  }

  void System_Window::HandleMessage(Message_Window_Hidden *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Exposed *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Moved *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Resized *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Minimized *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Maximized *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Restored *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Enter *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Leave *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Focus_Gained *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Focus_Lost *)
  {
    
  }

  void System_Window::HandleMessage(Message_Window_Close * a_pMsg)
  {
    Application::Instance()->RequestQuit();
    a_pMsg->SetFlag(Message::Flag::Handled, true);
  }

  void System_Window::HandleMessage(Message_Window_Take_Focus *)
  {

  }
}
