//@group Systems

#include "System_GUI.h"
#include "InputCodes.h"
#include "Options.h"
#include "BSR_Assert.h"
#include "GUI_Internal.h"
#include "Renderer.h"

namespace Engine
{
  System_GUI::System_GUI(int a_windowW, int a_windowH)
    : m_dt(1.0f / 60.0f)
    , m_pScreen(nullptr)
  {
    m_pScreen = GUI::Container::Create(nullptr, {0.f, 0.f}, {(float)a_windowW, (float)a_windowH}, {GUI::WidgetFlag::NoBackground});
    m_pScreen->SetContentMargin(0.0f);
  }

  System_GUI::~System_GUI()
  {
    delete m_pScreen;
  }

  void System_GUI::HandleMessage(Message * a_pMsg)
  {
    DISPATCH_MESSAGE(Message_Window_Resized);

    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    m_pScreen->HandleMessage(a_pMsg);

    a_pMsg->SetFlag(Message::Flag::Handled, true);
  }

  void System_GUI::HandleMessage(Message_Window_Resized * a_pMsg)
  {
    vec2 size((float)a_pMsg->w, (float)a_pMsg->h);
    GUI::Renderer::SetScreenSize(size);
    m_pScreen->SetSize(size);
  }

  void System_GUI::Update(float a_dt)
  {
    m_dt = a_dt;
  }

  void System_GUI::Render()
  {
    GlobalRenderState *pState = Renderer::GetGlobalRenderState();
    Renderer::Disable(RenderFeature::DepthTest);
    Renderer::Enable(RenderFeature::Sissor);
    m_pScreen->Draw();
    Renderer::SetRenderState(pState);
  }

  void System_GUI::ClearFrame()
  {
    m_pScreen->Clear();
  }

  void System_GUI::AddWidget(GUI::Widget * a_pWgt)
  {
    m_pScreen->Add(a_pWgt);
  }
}