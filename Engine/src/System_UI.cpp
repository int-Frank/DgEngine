//@group Systems

#include "System_UI.h"
#include "InputCodes.h"
#include "Options.h"
#include "core_Assert.h"
#include "UI_Internal.h"

namespace Engine
{
  System_UI::System_UI(int a_windowW, int a_windowH)
    : m_dt(1.0f / 60.0f)
    , m_pScreen(nullptr)
  {
    m_pScreen = UIWindow::Create(nullptr, {0.f, 0.f}, {(float)a_windowW, (float)a_windowH}, UIWindow::NoBackground);
  }

  System_UI::~System_UI()
  {
    delete m_pScreen;
  }

  void System_UI::HandleMessage(Message * a_pMsg)
  {
    DISPATCH_MESSAGE(Message_Window_Resized);

    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    m_pScreen->HandleMessage(a_pMsg);

    a_pMsg->SetFlag(Message::Flag::Handled, true);
  }

  void System_UI::HandleMessage(Message_Window_Resized * a_pMsg)
  {
    vec2 size((float)a_pMsg->w, (float)a_pMsg->h);
    UIRenderer::Instance()->SetScreenSize(size);
    m_pScreen->SetSize(size);
  }

  void System_UI::Update(float a_dt)
  {
    m_dt = a_dt;
  }

  void System_UI::Render()
  {
    m_pScreen->Draw();
  }

  void System_UI::ClearFrame()
  {
    m_pScreen->Clear();
  }

  void System_UI::AddWidget(UIWidget * a_pWgt)
  {
    m_pScreen->Add(a_pWgt);
  }
}