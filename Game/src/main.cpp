
#include <chrono>
#include <map>

#include "Engine.h"
#include "System_Input.h"
#include "System_GUI.h"
#include "Renderer.h"
#include "RT_RendererAPI.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RendererProgram.h"
#include "Material.h"
#include "Texture.h"
#include "InputCodes.h"

#include "GUI_Button.h"
#include "GUI_Text.h"
#include "GUI_Container.h"
#include "EngineMessages.h"
#include "DgBinPacker.h"
#include "DgRNG_Local.h"
#include "IFontAtlas.h"

#include "RenderDemo.h"

class Game : public Engine::Application
{
public:
  Game(Opts const & a_opts)
    : Application(a_opts)
  {
#define COMPILE_TESTS
#ifdef COMPILE_TESTS
    void RunTests();
    RunTests();
#endif

    PushSystem(new RenderDemo());

    Engine::System_GUI * pSysUI = static_cast<Engine::System_GUI *>(GetSystem(Engine::System_GUI::GetStaticID()));
    if (!pSysUI)
    {
      LOG_ERROR("Couldn't find input layer!");
    }
    else
    {
      /*Engine::GUI::Container * pWindow1a = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {200.f, 100.f});
      pWindow1a->SetBackgroundColour(Engine::Colour(0, 255, 0, 255));
      Engine::GUI::Button * pBtn1a = Engine::GUI::Button::Create(nullptr, "Hello", {10.f, 20.f}, {50.f, 20.f});
      pBtn1a->SetBackgroundColour(Engine::Colour(64, 0, 128, 255));
      pBtn1a->SetHoverOnBackgroundColour(Engine::Colour(128, 0, 255, 255));
      pBtn1a->BindSelect([](){LOG_WARN("PRESSED 1a");});
      pBtn1a->BindHoverOn([](){LOG_DEBUG("HOVER ON 1a");});
      pBtn1a->BindHoverOff([](){LOG_DEBUG("HOVER OFF 1a");});

      Engine::GUI::Container * pWindow1b = Engine::GUI::Container::Create(nullptr, {400.f, 20.f}, {200.f, 100.f});
      pWindow1b->SetBackgroundColour(Engine::Colour(0, 255, 255, 255));
      
      Engine::GUI::Container * pWindow1 = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {800.f, 400.f});
      pWindow1->SetBackgroundColour(Engine::Colour(255, 255, 255, 255));
      Engine::GUI::Button * pBtn1 = Engine::GUI::Button::Create(nullptr, "Hello", {10.f, 20.f}, {50.f, 20.f});
      pBtn1->SetBackgroundColour(Engine::Colour(64, 128, 0, 255));
      pBtn1->SetHoverOnBackgroundColour(Engine::Colour(128, 255, 0, 255));
      pBtn1->BindSelect([](){LOG_WARN("PRESSED 1");});
      pBtn1->BindHoverOn([](){LOG_DEBUG("HOVER ON 1");});
      pBtn1->BindHoverOff([](){LOG_DEBUG("HOVER OFF 1");});

      pWindow1a->Add(pBtn1a);
      pWindow1->Add(pBtn1);
      pWindow1->Add(pWindow1a);
      pWindow1->Add(pWindow1b);

      pSysUI->AddWidget(pWindow1);*/

      Engine::GUI::Container * pWindow = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {200.f, 200.f});
      pWindow->SetBackgroundColour(Engine::Colour(128, 0, 0, 128));

      Engine::GUI::TextAttributes attr = {};
      attr.size = DEFAULT_FONT_SIZE;
      attr.colourText = 0xFFFFFFFF;
      attr.lineSpacing = 1.0f;
      attr.horizontalAlign = Engine::GUI::HorizontalAlignment::Left;
      attr.verticalAlign = Engine::GUI::VerticalAlignment::Top;
      attr.wrapText = true;

      char const str[] = "The quick brown fox jumps over the lazy dog\n   \nAnd here is another line.   \n";
      //char const str[] = "T \n\n  ";

      Engine::GUI::Text * pText = Engine::GUI::Text::Create(nullptr, str, {100.0f, 100.0f}, {100.0f, 100.0f}, &attr, 
        {Engine::GUI::WidgetFlag::StretchHeight, Engine::GUI::WidgetFlag::StretchWidth});
      pWindow->Add(pText);
      pSysUI->AddWidget(pWindow);
    }

    Engine::System_Input * layer = static_cast<Engine::System_Input *>(GetSystem(Engine::System_Input::GetStaticID()));
    if (!layer)
      LOG_ERROR("Couldn't find input layer!");
    else
    {
      // TODO A helper function should live in the Engine
      // Hook up GUI events
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

    LOG_TRACE("Game initialised!");
    //RequestQuit();
  }

  ~Game()
  {

  }

};

Engine::Application* Engine::CreateApplication()
{
  Application::Opts opts;
  return new Game(opts);
}
