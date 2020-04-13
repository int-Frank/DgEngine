//@group Layers

#include "Memory.h"

#include "Layer_Window.h"
#include "EngineMessages.h"
#include "Application.h"
#include "IWindow.h"

namespace Engine
{
  Layer_Window::Layer_Window(Ref<IWindow> a_pWindow)
    : m_pWindow(a_pWindow)
  {

  }

  Layer_Window::~Layer_Window()
  {

  }

  void Layer_Window::OnAttach()
  {

  }

  void Layer_Window::OnDetach()
  {

  }

  void Layer_Window::Update(float a_dt)
  {
    m_pWindow->Update();
  }

  void Layer_Window::HandleMessage(Message* a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_Window)
      return;

    DISPATCH_MESSAGE(Message_Window_Shown);
    DISPATCH_MESSAGE(Message_Window_Hidden);
    DISPATCH_MESSAGE(Message_Window_Exposed);
    DISPATCH_MESSAGE(Message_Window_Moved);
    DISPATCH_MESSAGE(Message_Window_Resized);
    DISPATCH_MESSAGE(Message_Window_Size_Changed);
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

  void Layer_Window::HandleMessage(Message_Window_Shown*)
  {

  }

  void Layer_Window::HandleMessage(Message_Window_Hidden *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Exposed *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Moved *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Resized *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Size_Changed *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Minimized *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Maximized *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Restored *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Enter *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Leave *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Focus_Gained *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Focus_Lost *)
  {
    
  }

  void Layer_Window::HandleMessage(Message_Window_Close * a_pMsg)
  {
    Application::Instance()->RequestQuit();
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void Layer_Window::HandleMessage(Message_Window_Take_Focus *)
  {

  }
}
