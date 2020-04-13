//@group Layers

#include "Layer_UI.h"
#include "InputCodes.h"
#include "Options.h"
#include "core_Assert.h"

namespace Engine
{
  Layer_UI::Layer_UI()
    : m_dt(1.0f / 60.0f)
  {
  }

  Layer_UI::~Layer_UI()
  {
  }

  void Layer_UI::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    DISPATCH_MESSAGE(Message_GUI_MouseMove);
    DISPATCH_MESSAGE(Message_GUI_MouseButtonDown);
    DISPATCH_MESSAGE(Message_GUI_MouseButtonUp);
    DISPATCH_MESSAGE(Message_GUI_KeyDown);
    DISPATCH_MESSAGE(Message_GUI_KeyUp);
    DISPATCH_MESSAGE(Message_GUI_MouseWheelDown);
    DISPATCH_MESSAGE(Message_GUI_MouseWheelUp);
    DISPATCH_MESSAGE(Message_GUI_Text);
  }

  void Layer_UI::HandleMessage(Message_GUI_MouseMove * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_MouseButtonDown * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_MouseButtonUp * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_KeyDown * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_KeyUp * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_MouseWheelUp * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_MouseWheelDown * a_pMsg)
  {

  }

  void Layer_UI::HandleMessage(Message_GUI_Text * a_pMsg)
  {

  }

  void Layer_UI::Update(float a_dt)
  {
    m_dt = a_dt;
  }

  void Layer_UI::Render()
  {

  }
}