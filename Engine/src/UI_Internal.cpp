//@group UI

#include "UI_Internal.h"

#include "core_Assert.h"
#include "Material.h"
#include "ShaderUniform.h"
#include "RendererProgram.h"
#include "VertexArray.h"
#include "Renderer.h"

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
        vec2 xy = ((inPos * buttonSize + buttonPos)  / windowSize  - vec2(0.5, 0.5)) * 2.0;
        xy.y = -xy.y;
        gl_Position = vec4(xy, 0.0, 1.0);
      })";

  static char const * g_box_fs = R"(
      #version 430
      uniform vec4 colour;
      out vec4 FragColour;
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

    Ref<Material>     material;
    Ref<VertexBuffer> vb;
    Ref<IndexBuffer>  ib;
    Ref<VertexArray>  va;
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
    BSR_ASSERT(s_pInstance != nullptr);
    return s_pInstance;
  }

  UIRenderer::UIRenderer()
    : m_pimpl(new PIMPL())
  {
    m_pimpl->vb = VertexBuffer::Create(g_boxVerts, SIZEOF32(g_boxVerts));
    m_pimpl->vb->SetLayout(
      {
        { Engine::ShaderDataType::VEC2, "inPos" }
      });

    m_pimpl->ib = Engine::IndexBuffer::Create(g_boxIndices, SIZEOF32(g_boxIndices));
    m_pimpl->va = Engine::VertexArray::Create();

    m_pimpl->va->AddVertexBuffer(m_pimpl->vb);
    m_pimpl->va->SetIndexBuffer(m_pimpl->ib);

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

  void UIRenderer::Bind()
  {
    m_pimpl->material->Bind(); // TODO this material is specific to drawing boxes and should be moved to DrawBox()
  }

  void UIRenderer::Unbind()
  {
    // ?? How to unbind / restore state?
  }

  void UIRenderer::SetScreenSize(vec2 const & a_size)
  {
    m_pimpl->material->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
  }

  void UIRenderer::DrawBox(vec2 const & a_position, vec2 const & a_size, Colour a_colour)
  {
    float clr[4] = {a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

    m_pimpl->material->SetUniform("colour", clr, sizeof(clr));
    m_pimpl->material->SetUniform("buttonPos", a_position.GetData(), sizeof(a_position));
    m_pimpl->material->SetUniform("buttonSize", a_size.GetData(), sizeof(a_size));

    m_pimpl->material->Bind();
    m_pimpl->va->Bind();

    Renderer::DrawIndexed(6, false);
  }

  void UIRenderer::DrawRoundedBox(vec2 const & a_position, vec2 const & a_size, Colour a_colour, float a_radius)
  {
  
  }
}