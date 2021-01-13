
#include "Options.h"
#include "GUIDemo.h"
#include "Application.h"
#include "System_GUI.h"
#include "System_Input.h"

#include "GUI.h"
#include "GUI_Container.h"
#include "GUI_Button.h"
#include "GUI_Text.h"
#include "GUI_Checkbox.h"
#include "GUI_Slider.h"

using namespace Engine;
using namespace Engine::GUI;

MAKE_SYSTEM_DEFINITION(GUIDemo)

void GUIDemo::OnAttach()
{
  BuildGUI();
  BindInput();
}

void GUIDemo::BuildGUI()
{
  System_GUI * pSysUI = static_cast<System_GUI *>(Application::Instance()->GetSystem(System_GUI::GetStaticID()));
  if (!pSysUI)
  {
    LOG_ERROR("Couldn't find input layer!");
    return;
  }


  // Main window
  Container * pMainWindow = Container::Create(nullptr, {20.f, 20.f}, {768.f, 512.f});

  // Controls
  Container * pControlsWindow = Container::Create(nullptr, {20.f, 20.f}, {250.f, 200.f});
  Button * pBtnReset = Button::Create(nullptr, "Reset", {10.f, 20.f}, {70.f, 30.f});
  pBtnReset->BindSelect([](){LOG_WARN("PRESSED");});
  pControlsWindow->Add(pBtnReset);

  // Text window
  const char * pStr = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum";
  Container * pTextWindow = Container::Create(nullptr, {400.f, 20.f}, {300.f, 200.f});
  TextAttributes attr ={};
  attr.size = GUI_FONT_SIZE; // TODO This should not be exposed
  attr.colourText = GetStyle().colours[col_Text];
  attr.lineSpacing = GetStyle().textLineSpacing;
  attr.horizontalAlign = Engine::GUI::HorizontalAlignment::Centre;
  attr.verticalAlign = Engine::GUI::VerticalAlignment::Centre;
  attr.wrapText = true;
  Text *pText = Text::Create(pTextWindow, pStr, {0.0, 0.0}, {0.0, 0.0}, &attr, {WidgetFlag::StretchHeight, WidgetFlag::StretchWidth, WidgetFlag::NotResponsive});

  CheckBox *pChkboxWrap = CheckBox::Create(pControlsWindow, {20.0f, 100.0f});
  pChkboxWrap->BindChangeSelected([](bool a_selected)
    {
      if (a_selected)
        LOG_DEBUG("ON");
      else
        LOG_DEBUG("OFF");
    });
  pControlsWindow->Add(pChkboxWrap);

  SliderFloat *pSlider = SliderFloat::Create(pControlsWindow, {20.0f, 150.0f}, 128.0f, 128.0f, 0.0f, 255.0f);
  pSlider->BindNewValue([](float a_val)
    {
      LOG_DEBUG("Slider: {}", a_val);
    });
  pControlsWindow->Add(pSlider);

  // Hooking it all up
  pTextWindow->Add(pText);
  pMainWindow->Add(pTextWindow);
  pMainWindow->Add(pControlsWindow);
  pSysUI->AddWidget(pMainWindow);
}

void GUIDemo::BindInput()
{
  System_Input * layer = static_cast<Engine::System_Input *>(Application::Instance()->GetSystem(Engine::System_Input::GetStaticID()));
  if (!layer)
  {
    LOG_ERROR("Couldn't find input layer!");
    return;
  }

  // TODO A helper function should live in the Engine to set up default GUI bindings
  layer->AddBinding(Engine::Message_Input_MouseMove::GetStaticID(),
    [](Engine::Message const * pMsg)
    {
      Engine::Message_Input_MouseMove * pIn = (Engine::Message_Input_MouseMove *)pMsg;
      Engine::Message_GUI_PointerMove * pOut = nullptr;
      EMPLACE_POST(Engine::Message_GUI_PointerMove, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });

  layer->AddBinding(Engine::Message_Input_KeyDown::GetStaticID(),
    [](Engine::Message const * pMsg)
    {
      Engine::Message_Input_KeyDown * pIn = (Engine::Message_Input_KeyDown *)pMsg;
      if (pIn->keyCode == Engine::IC_KEY_Q && (pIn->modState & KM_ALT) != 0)
        EMPLACE_POST(Engine::Message_Quit);
    });

  layer->AddBinding(Engine::Message_Input_MouseButtonDown::GetStaticID(),
    [](Engine::Message const * pMsg)
    {
      Engine::Message_Input_MouseButtonDown * pIn = (Engine::Message_Input_MouseButtonDown *)pMsg;
      if (pIn->button != Engine::IC_MOUSE_BUTTON_LEFT)
        return;

      Engine::Message_GUI_PointerDown * pOut = nullptr;
      EMPLACE_POST(Engine::Message_GUI_PointerDown, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });

  layer->AddBinding(Engine::Message_Input_MouseButtonUp::GetStaticID(),
    [](Engine::Message const * pMsg)
    {
      Engine::Message_Input_MouseButtonUp * pIn = (Engine::Message_Input_MouseButtonUp *)pMsg;
      if (pIn->button != Engine::IC_MOUSE_BUTTON_LEFT)
        return;

      Engine::Message_GUI_PointerUp * pOut = nullptr;
      EMPLACE_POST(Engine::Message_GUI_PointerUp, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });
}