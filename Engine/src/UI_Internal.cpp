//@group UI

#include "UI_Internal.h"

#include "core_Assert.h"
#include "Material.h"
#include "ShaderUniform.h"
#include "RendererProgram.h"

namespace Engine
{
  enum UIMesh
  {
    uiM_Box,
    uiM_RoundedBox,

    uiM_COUNT
  };

  static char const * g_box_vs = R"(
      #version 430
      layout(location = 0) in vec2 inPos;
      uniform vec2 windowSize;
      uniform vec2 buttonPos;
      uniform vec2 buttonSize;
      void main()
      {
        vec2 xy = ((inPos * buttonSize + buttonPos) / windowSize - vec2(0.5, 0.5)) * 2.0;
        gl_Position = vec4(xy, 0.0, 1.0);
      })";

  static char const * g_box_fs = R"(
      #version 430
      uniform vec4 colour;
      out vec4 FragColor;
      void main()
      {
        FragColour = colour;
      })";

  static float const g_boxVerts[] =
  {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
  };

  static int const g_boxIndices[] ={0, 1, 2, 0, 2, 3};

  class UIRenderer::PIMPL
  {
  public:

    Ref<Material> material;
  };

  UIRenderer * UIRenderer::s_pInstance = nullptr;

  bool UIRenderer::Init()
  {
    BSR_ASSERT(s_pInstance == nullptr, "UIRenderer already initialised!");
    s_pInstance = new UIRenderer();
    return true;
  }

  void UIRenderer::Destroy()
  {
    delete s_pInstance;
    s_pInstance = nullptr;
  }

  UIRenderer * UIRenderer::Instance()
  {
    return s_pInstance;
  }

  UIRenderer::UIRenderer()
    : m_pimpl(new PIMPL())
  {
    Engine::ShaderData * pSD = new ShaderData({
        { Engine::ShaderDomain::Vertex, Engine::StrType::Source, g_box_vs },
        { Engine::ShaderDomain::Fragment, Engine::StrType::Source, g_box_fs }
      });

    ResourceManager::Instance()->RegisterResource(ir_GUIShaderData, pSD);
    Ref<Engine::RendererProgram> refProg;
    refProg = Engine::RendererProgram::Create(ir_GUIShaderData);
    m_pimpl->material = Material::Create(refProg);
  }

  UIRenderer::~UIRenderer()
  {
    delete m_pimpl;
  }
}