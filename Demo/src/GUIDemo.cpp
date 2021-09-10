
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

using namespace DgE;
using namespace DgE::GUI;

MAKE_SYSTEM_DEFINITION(GUIDemo)

void GUIDemo::OnAttach()
{
  m_textColour = 0xFFFFFFFF;

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
  Container * pMainWindow = Container::Create(nullptr, {70.f, 40.f}, {735.f, 450.f});

  // Text window
  const char * pStr = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\nDuis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\nExcepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum";
  Container * pTextWindow = Container::Create(nullptr, {200.f, 10.f}, {480.f, 280.f});
  TextAttributes attr ={};
  attr.size = GUI_FONT_SIZE; // TODO This should not be exposed
  attr.colourText = GetStyle().colours[col_Text];
  attr.lineSpacing = GetStyle().textLineSpacing;
  attr.horizontalAlign = DgE::GUI::HorizontalAlignment::Left;
  attr.verticalAlign = DgE::GUI::VerticalAlignment::Top;
  attr.wrapText = true;
  Text * pText = Text::Create(pTextWindow, pStr, {0.0f, 0.0f}, {0.0f, 0.0f}, &attr, {WidgetFlag::StretchHeight, WidgetFlag::StretchWidth, WidgetFlag::NotResponsive});

  // Control Window
  Container * pControlsWindow = Container::Create(nullptr, {10.f, 10.f}, {150.f, 280.f});

  // Text wrapping options
  CheckBox *pChkboxWrap = CheckBox::Create(pControlsWindow, {12.0f, 60.0f}, true);
  pChkboxWrap->BindCheckedChanged([pText](bool a_selected)
    {
      pText->SetWrap(a_selected);
    });

  Text *pWrapLabel = Text::Create(pControlsWindow, "Wrap text", {50.0f, 60.0f}, {200.0f, 40.0f}, nullptr, {WidgetFlag::NotResponsive});
  pControlsWindow->Add(pChkboxWrap);
  pControlsWindow->Add(pWrapLabel);

  // Colour sliders
  Text * pColourLabel = Text::Create(pControlsWindow, "Text colour", {25.0f, 110.0f}, {200.0f, 40.0f}, nullptr, {WidgetFlag::NotResponsive});
  pControlsWindow->Add(pColourLabel);

  Colour *pClr = &m_textColour;
  SliderInt * pRedSlider = SliderInt::Create(pControlsWindow, {10.0f, 150.0f}, 128.0f, 0, 255, 255);
  pRedSlider->SetColour(SliderState::Normal, SliderElement::Caret, 0xFF0000DD);
  pRedSlider->SetColour(SliderState::Hover, SliderElement::Caret, 0xFF0000EE);
  pRedSlider->SetColour(SliderState::Grab, SliderElement::Caret, 0xFF0000FF);
  pRedSlider->SetColour(SliderState::Normal, SliderElement::Lower, 0xFF0000EE);
  pRedSlider->SetColour(SliderState::Hover, SliderElement::Lower, 0xFF0000EE);
  pRedSlider->SetColour(SliderState::Grab, SliderElement::Lower, 0xFF0000EE);
  pRedSlider->BindNewValue([pText, pClr = &m_textColour] (int32_t a_val)
    {
      pClr->r(a_val);
      pText->SetColour(*pClr);
    });

  SliderInt * pBlueSlider = SliderInt::Create(pControlsWindow, {10.0f, 180.0f}, 128.0f, 0, 255, 255);
  pBlueSlider->SetColour(SliderState::Normal, SliderElement::Caret, 0xFFDD0000);
  pBlueSlider->SetColour(SliderState::Hover, SliderElement::Caret, 0xFFEE0000);
  pBlueSlider->SetColour(SliderState::Grab, SliderElement::Caret, 0xFFFF0000);
  pBlueSlider->SetColour(SliderState::Normal, SliderElement::Lower, 0xFFEE0000);
  pBlueSlider->SetColour(SliderState::Hover, SliderElement::Lower, 0xFFEE0000);
  pBlueSlider->SetColour(SliderState::Grab, SliderElement::Lower, 0xFFEE0000);
  pBlueSlider->BindNewValue([pText, pClr = &m_textColour](int32_t a_val)
    {
      pClr->b(a_val);
      pText->SetColour(*pClr);
    });

  SliderInt * pGreenSlider = SliderInt::Create(pControlsWindow, {10.0f, 210.0f}, 128.0f, 0, 255, 255);
  pGreenSlider->SetColour(SliderState::Normal, SliderElement::Caret, 0xFF00DD00);
  pGreenSlider->SetColour(SliderState::Hover, SliderElement::Caret, 0xFF00EE00);
  pGreenSlider->SetColour(SliderState::Grab, SliderElement::Caret, 0xFF00FF00);
  pGreenSlider->SetColour(SliderState::Normal, SliderElement::Lower, 0xFF00EE00);
  pGreenSlider->SetColour(SliderState::Hover, SliderElement::Lower, 0xFF00EE00);
  pGreenSlider->SetColour(SliderState::Grab, SliderElement::Lower, 0xFF00EE00);
  pGreenSlider->BindNewValue([pText, pClr = &m_textColour](int32_t a_val)
    {
      pClr->g(a_val);
      pText->SetColour(*pClr);
    });

  SliderInt * pAlphaSlider = SliderInt::Create(pControlsWindow, {10.0f, 240.0f}, 128.0f, 0, 255, 255);
  pAlphaSlider->BindNewValue([pText, pClr = &m_textColour](int32_t a_val)
    {
      pClr->a(a_val);
      pText->SetColour(*pClr);
    });

  pControlsWindow->Add(pRedSlider);
  pControlsWindow->Add(pGreenSlider);
  pControlsWindow->Add(pBlueSlider);
  pControlsWindow->Add(pAlphaSlider);

  // Reset button
  Button * pBtnReset = Button::Create(nullptr, "Reset", {10.f, 10.f}, {70.f, 30.f});
  pBtnReset->BindSelect([pChkboxWrap, pRedSlider, pGreenSlider, pBlueSlider, pAlphaSlider]()
    {
      pChkboxWrap->SetChecked(true);
      pRedSlider->SetValue(255);
      pGreenSlider->SetValue(255);
      pBlueSlider->SetValue(255);
      pAlphaSlider->SetValue(255);
    });
  pControlsWindow->Add(pBtnReset);

  // Hooking it all up
  pTextWindow->Add(pText);
  pMainWindow->Add(pTextWindow);
  pMainWindow->Add(pControlsWindow);
  pSysUI->AddWidget(pMainWindow);
}

void GUIDemo::BindInput()
{
  System_Input * layer = static_cast<DgE::System_Input *>(Application::Instance()->GetSystem(DgE::System_Input::GetStaticID()));
  if (!layer)
  {
    LOG_ERROR("Couldn't find input layer!");
    return;
  }

  // TODO A helper function should live in the Engine to set up default GUI bindings
  layer->AddBinding(DgE::IC_MOUSE_MOTION, DgE::IE_VALUE_CHANGE,
    [](DgE::Message const * pMsg, void *)
    {
      DgE::Message_Input_Mouse * pIn = (DgE::Message_Input_Mouse *)pMsg;
      DgE::Message_GUI_PointerMove * pOut = nullptr;
      EMPLACE_POST(DgE::Message_GUI_PointerMove, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });

  layer->AddBinding(DgE::IC_KEY_Q, DgE::IE_BUTTON_DOWN,
    [](DgE::Message const *, void *)
    {
      EMPLACE_POST(DgE::Message_Quit);
    });

  layer->AddBinding(DgE::IC_MOUSE_BUTTON_LEFT, DgE::IE_BUTTON_DOWN,
    [](DgE::Message const * pMsg, void *)
    {
      DgE::Message_Input_Mouse * pIn = (DgE::Message_Input_Mouse *)pMsg;
      DgE::Message_GUI_PointerDown * pOut = nullptr;
      EMPLACE_POST(DgE::Message_GUI_PointerDown, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });

  layer->AddBinding(DgE::IC_MOUSE_BUTTON_LEFT, DgE::IE_BUTTON_UP,
    [](DgE::Message const * pMsg, void *)
    {
      DgE::Message_Input_Mouse * pIn = (DgE::Message_Input_Mouse *)pMsg;
      DgE::Message_GUI_PointerUp * pOut = nullptr;
      EMPLACE_POST(DgE::Message_GUI_PointerUp, pOut);
      pOut->x = pIn->x;
      pOut->y = pIn->y;
    });
}