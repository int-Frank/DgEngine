
#include "Options.h"
#include "GUIDemo.h"
#include "Application.h"
#include "System_GUI.h"
#include "System_Input.h"

#include "GUI.h"
#include "GUI_Container.h"
#include "GUI_Button.h"
#include "GUI_Text.h"

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

  const char * pStr = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum";

  Container * pMainWindow = Container::Create(nullptr, {20.f, 20.f}, {768.f, 512.f});
  Container * pTextWindow = Container::Create(nullptr, {400.f, 20.f}, {300.f, 200.f});

  TextAttributes attr ={};
  attr.size = GUI_FONT_SIZE;
  attr.colourText = GetStyle().colours[col_Text];
  attr.lineSpacing = GetStyle().textLineSpacing;
  attr.horizontalAlign = Engine::GUI::HorizontalAlignment::Centre;
  attr.verticalAlign = Engine::GUI::VerticalAlignment::Centre;
  attr.wrapText = true;
  Text *pText = Text::Create(pTextWindow, pStr, {0.0, 0.0}, {0.0, 0.0}, &attr, {WidgetFlag::StretchHeight, WidgetFlag::StretchWidth, WidgetFlag::NotResponsive});

  pTextWindow->Add(pText);
  pMainWindow->Add(pTextWindow);
  pSysUI->AddWidget(pMainWindow);

  /*char const str[] ={0xEE, 0x80, 0x80};
  Button * pBtn = Button::Create(nullptr, str, {10.f, 20.f}, {16.f, 16.f});
  pBtn->BindSelect([](){LOG_WARN("PRESSED");});
  pBtn->BindHoverOn([](){LOG_DEBUG("HOVER ON");});
  pBtn->BindHoverOff([](){LOG_DEBUG("HOVER OFF");});

  pMain->Add(pBtn);*/

  //pWindow1a->SetBackgroundColour(Engine::Colour(0, 255, 0, 255));
  //
  //
  //Engine::GUI::Container * pWindow1a = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {200.f, 100.f});
  //Engine::GUI::Button * pBtn1a = Engine::GUI::Button::Create(nullptr, "Hello", {10.f, 20.f}, {50.f, 20.f});
  //pBtn1a->SetBackgroundColour(Engine::Colour(64, 0, 128, 255));
  //pBtn1a->SetHoverOnBackgroundColour(Engine::Colour(128, 0, 255, 255));
  //pBtn1a->BindSelect([](){LOG_WARN("PRESSED 1a");});
  //pBtn1a->BindHoverOn([](){LOG_DEBUG("HOVER ON 1a");});
  //pBtn1a->BindHoverOff([](){LOG_DEBUG("HOVER OFF 1a");});
  //
  //Engine::GUI::Container * pWindow1b = Engine::GUI::Container::Create(nullptr, {400.f, 20.f}, {200.f, 100.f});
  //pWindow1b->SetBackgroundColour(Engine::Colour(0, 255, 255, 255));
  //
  //Engine::GUI::Container * pWindow1 = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {800.f, 400.f});
  //pWindow1->SetBackgroundColour(Engine::Colour(255, 255, 255, 255));
  //Engine::GUI::Button * pBtn1 = Engine::GUI::Button::Create(nullptr, "Hello", {10.f, 20.f}, {50.f, 20.f});
  //pBtn1->SetBackgroundColour(Engine::Colour(64, 128, 0, 255));
  //pBtn1->SetHoverOnBackgroundColour(Engine::Colour(128, 255, 0, 255));
  //pBtn1->BindSelect([](){LOG_WARN("PRESSED 1");});
  //pBtn1->BindHoverOn([](){LOG_DEBUG("HOVER ON 1");});
  //pBtn1->BindHoverOff([](){LOG_DEBUG("HOVER OFF 1");});
  //
  //pWindow1a->Add(pBtn1a);
  //pWindow1->Add(pBtn1);
  //pWindow1->Add(pWindow1a);
  //pWindow1->Add(pWindow1b);
  //
  //pSysUI->AddWidget(pWindow1);
  //
  //Engine::GUI::Container * pWindow = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {200.f, 200.f});
  //pWindow->SetBackgroundColour(Engine::Colour(128, 0, 0, 128));
  //
  //Engine::GUI::TextAttributes attr ={};
  //attr.size = DEFAULT_FONT_SIZE;
  //attr.colourText = 0xFFFFFFFF;
  //attr.lineSpacing = 1.0f;
  //attr.horizontalAlign = Engine::GUI::HorizontalAlignment::Centre;
  //attr.verticalAlign = Engine::GUI::VerticalAlignment::Top;
  //attr.wrapText = true;
  //
  ////char const str[] = "The quick brown a fox jumps over the lazy dog\n   \nAnd here is another line.   ";
  //char const str[] ={0xE2, 0x9C, 0x94, 0xE2, 0x97, 0x89, 0xE2, 0x97, 0x8B, 0xE2, 0x98, 0x90, 0xEE, 0x80, 0x80};
  ////char const str[] = "       T";
  //
  //Engine::GUI::Text * pText = Engine::GUI::Text::Create(nullptr, str, {100.0f, 100.0f}, {100.0f, 100.0f}, &attr,
  //  {Engine::GUI::WidgetFlag::StretchHeight, Engine::GUI::WidgetFlag::StretchWidth, Engine::GUI::WidgetFlag::NotResponsive});
  //pWindow->Add(pText);
  //pSysUI->AddWidget(pWindow);
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