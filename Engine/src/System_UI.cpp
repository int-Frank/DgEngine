//@group Systems

#include "System_UI.h"
#include "InputCodes.h"
#include "Options.h"
#include "core_Assert.h"
#include "UI_Internal.h"

namespace Engine
{
  System_UI::System_UI()
    : m_dt(1.0f / 60.0f)
  {

  }

  System_UI::~System_UI()
  {

  }

  void System_UI::HandleMessage(Message * a_pMsg)
  {
    DISPATCH_MESSAGE(Message_Window_Resized);

    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    m_frame.HandleMessage(a_pMsg);

    a_pMsg->SetFlag(Message::Flag::Handled, true);
  }

  void System_UI::HandleMessage(Message_Window_Resized * a_pMsg)
  {
    UIRenderer::Instance()->SetScreenSize(vec2((float)a_pMsg->w, (float)a_pMsg->h));
  }

  void System_UI::Update(float a_dt)
  {
    m_dt = a_dt;
  }

  void System_UI::Render()
  {
    m_frame.Draw();
  }

  void System_UI::ClearFrame()
  {
    m_frame.Clear();
  }

  void System_UI::AddWidget(UIWidget * a_pWgt)
  {
    m_frame.Add(a_pWgt);
  }
}