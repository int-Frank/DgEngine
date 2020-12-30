
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

Engine::Colour * GenerateTexture()
{
  Engine::Colour *pPixels = new Engine::Colour[FONTATLAS_DEFAULT_TEXTURE_DIMENSION * FONTATLAS_DEFAULT_TEXTURE_DIMENSION]{};
  for (size_t i = 0; i < FONTATLAS_DEFAULT_TEXTURE_DIMENSION * FONTATLAS_DEFAULT_TEXTURE_DIMENSION; i++)
  {
    Engine::Colour c;
    c.r(255);
    c.g(255);
    c.b(255);
    c.a(g_pPixels_DEBUG[i]);
    pPixels[i] = c;
  }
  return pPixels;
}

class GameSystem : public Engine::System
{
public:

  ASSIGN_ID(256);
  GameSystem()
  {

  }

  void OnAttach() override
  {
    Engine::Renderer::SetClearColor(1.0f, 0.0f, 1.0f);

    float verts[] = 
    {
      -1.0f, 1.0f, 0.0f, 0.0f,
      0.5f, 1.0f,  1.0, 0.0,
      0.5f, -0.66666f,  1.0, 1.0,
      -1.0f, -0.66666f, 0.0, 1.0
    };

    int indices[] = {0, 1, 2, 0, 2, 3};

    m_vb = Engine::VertexBuffer::Create(verts, SIZEOF32(verts));
    m_vb->SetLayout(
      {
        { Engine::ShaderDataType::VEC2, "inPos" }, //TODO remove these strings.
        { Engine::ShaderDataType::VEC2, "inTexCoord" },
      });

    m_ib = Engine::IndexBuffer::Create(indices, SIZEOF32(indices));
    m_va = Engine::VertexArray::Create();

    m_va->AddVertexBuffer(m_vb);
    m_va->SetIndexBuffer(m_ib);

    Engine::ShaderData *pSD = new Engine::ShaderData({
        { Engine::ShaderDomain::Vertex, Engine::StrType::Path, "src/vs.glsl" },
        { Engine::ShaderDomain::Fragment, Engine::StrType::Path, "src/fs.glsl" }
      });

    Engine::ResourceID sdID = 12345;
    Engine::ResourceManager::Instance()->RegisterResource(sdID, pSD);

    Engine::Ref<Engine::RendererProgram> refProg;
    refProg = Engine::RendererProgram::Create(sdID);

    Engine::TextureAttributes attrs;
    attrs.SetFilter(Engine::TextureFilter::Linear);
    attrs.SetIsMipmapped(false);
    attrs.SetWrap(Engine::TextureWrap::Clamp);
    attrs.SetPixelType(Engine::TexturePixelType::RGBA8);
    m_texture = Engine::Texture2D::Create();
    m_texture->Set(FONTATLAS_DEFAULT_TEXTURE_DIMENSION, FONTATLAS_DEFAULT_TEXTURE_DIMENSION, GenerateTexture(), attrs);
    m_texture->Upload();

    m_material = Engine::Material::Create(refProg);

    m_material->SetTexture("texture1", m_texture);
    m_material->Bind();
  }

  void HandleMessage(Engine::Message* a_pMsg) override
  {

  }

  void OnDetach() override
  {

  }

  void Update(float a_dt) override
  {
    //std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //LOG_DEBUG("ASDF");
  }

  void Render() override
  {
    Engine::Renderer::Clear(1.0f, 0.0f, 1.0f);

    m_material->Bind();
    m_va->Bind();

    Engine::Renderer::DrawIndexed(6, false);

  }

private:

  Engine::Ref<Engine::VertexBuffer>     m_vb;
  Engine::Ref<Engine::IndexBuffer>      m_ib;
  Engine::Ref<Engine::VertexArray>      m_va;
  Engine::Ref<Engine::Texture2D>        m_texture;
  Engine::Ref<Engine::Material>         m_material;
};

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

    PushSystem(new GameSystem());

    Engine::System_GUI * pSysUI = static_cast<Engine::System_GUI *>(GetSystem(Engine::System_GUI::GetStaticID()));
    if (!pSysUI)
    {
      LOG_ERROR("Couldn't find input layer!");
    }
    else
    {
      Engine::GUI::Container * pWindow1a = Engine::GUI::Container::Create(nullptr, {20.f, 20.f}, {200.f, 100.f});
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

      pSysUI->AddWidget(pWindow1);

      //Engine::GUI::Text * pText = Engine::GUI::Text::Create(nullptr, "Text", {100.0f, 100.0f}, {100.0f, 100.0f});
      //pSysUI->AddWidget(pText);

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
