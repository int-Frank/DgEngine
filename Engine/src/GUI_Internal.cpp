//@group UI

#include "GUI_Internal.h"

#include "BSR_Assert.h"
#include "Framework.h"
#include "Material.h"
#include "ShaderUniform.h"
#include "RendererProgram.h"
#include "VertexArray.h"
#include "Renderer.h"
#include <algorithm>

#define DEFAULT_FONT_PATH "../Engine/assets/fonts/NotoSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 58

namespace Engine
{
  namespace GUI
  {
    enum Mesh
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

    static int const g_boxIndices[] ={1, 2, 3, 3, 0, 1};

    struct Renderable
    {
      Ref<Material>     material;
      Ref<VertexBuffer> vb;
      Ref<IndexBuffer>  ib;
      Ref<VertexArray>  va;
    };

    class Renderer::PIMPL
    {
    public:

      PIMPL();
      ~PIMPL();

      Dg::ErrorCode Init();

      void SetScreenSize(vec2 const &);
      void DrawText(std::string const &, UIAABB const &, TextOptions const &);
      void DrawText(std::string const &, UIAABB const &, TextOptions const &, Ref<IFontAtlas>, FontID);
      void DrawBox(UIAABB const &, Colour colour);

    private:

      void InitBox();

    private:

      FontID m_defaultFont;
      Ref<IFontAtlas> m_fontAtlas;
      Renderable m_renBox;
    };

    //------------------------------------------------------------------------
    // PIMPL
    //------------------------------------------------------------------------

    Renderer * Renderer::s_pInstance = nullptr;

    Renderer::PIMPL::PIMPL()
      : m_defaultFont(0)
    {

    }

    Renderer::PIMPL::~PIMPL()
    {

    }

    Dg::ErrorCode Renderer::PIMPL::Init()
    {
      Dg::ErrorCode result;

      InitBox();
      m_fontAtlas = Framework::Instance()->CreateFontAtlas();

      DG_ERROR_CHECK(m_fontAtlas->RegisterFont(DEFAULT_FONT_PATH, m_defaultFont));
      m_fontAtlas->BeginLoad();
      DG_ERROR_CHECK(m_fontAtlas->RegisterAllGlyphs(m_defaultFont, DEFAULT_FONT_SIZE));
      DG_ERROR_CHECK(m_fontAtlas->CommitLoad());

      result = Dg::ErrorCode::None;
    epilogue:
      return result;
    }

    void Renderer::PIMPL::InitBox()
    {
      m_renBox.vb = VertexBuffer::Create(g_boxVerts, SIZEOF32(g_boxVerts));
      m_renBox.vb->SetLayout(
        {
          { Engine::ShaderDataType::VEC2, "inPos" }
        });

      m_renBox.ib = Engine::IndexBuffer::Create(g_boxIndices, SIZEOF32(g_boxIndices));
      m_renBox.va = Engine::VertexArray::Create();

      m_renBox.va->AddVertexBuffer(m_renBox.vb);
      m_renBox.va->SetIndexBuffer(m_renBox.ib);

      Engine::ShaderData * pSD = new ShaderData({
          { Engine::ShaderDomain::Vertex, Engine::StrType::Source, g_box_vs },
          { Engine::ShaderDomain::Fragment, Engine::StrType::Source, g_box_fs }
        });

      ResourceManager::Instance()->RegisterResource(ir_GUIShaderData, pSD);
      Ref<Engine::RendererProgram> refProg;
      refProg = Engine::RendererProgram::Create(ir_GUIShaderData);
      m_renBox.material = Material::Create(refProg);
    }

    void Renderer::PIMPL::SetScreenSize(vec2 const & a_size)
    {
      m_renBox.material->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
    }

    void Renderer::PIMPL::DrawBox(UIAABB const & a_aabb, Colour a_colour)
    {
      float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

      m_renBox.material->SetUniform("colour", clr, sizeof(clr));
      m_renBox.material->SetUniform("buttonPos", a_aabb.position.GetData(), sizeof(a_aabb.position));
      m_renBox.material->SetUniform("buttonSize", a_aabb.size.GetData(), sizeof(a_aabb.size));

      m_renBox.material->Bind();
      m_renBox.va->Bind();

      ::Engine::Renderer::DrawIndexed(6, false);
    }

    void Renderer::PIMPL::DrawText(std::string const & a_str, UIAABB const & a_aabb, TextOptions const & a_options)
    {

    }

    void Renderer::PIMPL::DrawText(std::string const & a_str, UIAABB const & a_aabb, TextOptions const & a_options, Ref<IFontAtlas> a_fontAtlas, FontID a_id)
    {

    }

    //------------------------------------------------------------------------
    // Renderer
    //------------------------------------------------------------------------

    Dg::ErrorCode Renderer::Init()
    {
      BSR_ASSERT(s_pInstance == nullptr, "Renderer already initialised!");
      s_pInstance = new Renderer();
      return s_pInstance->m_pimpl->Init();
    }

    void Renderer::Destroy()
    {
      delete s_pInstance;
      s_pInstance = nullptr;
    }

    Renderer * Renderer::Instance()
    {
      BSR_ASSERT(s_pInstance != nullptr);
      return s_pInstance;
    }

    Renderer::Renderer()
      : m_pimpl(new PIMPL())
    {

    }

    Renderer::~Renderer()
    {
      delete m_pimpl;
    }

    void Renderer::SetScreenSize(vec2 const & a_size)
    {
      m_pimpl->SetScreenSize(a_size);
    }

    void Renderer::DrawText(std::string const & a_str, UIAABB const & a_aabb, TextOptions const & a_options)
    {
      m_pimpl->DrawText(a_str, a_aabb, a_options);
    }

    void Renderer::DrawText(std::string const & a_str, UIAABB const & a_aabb, TextOptions const & a_options, Ref<IFontAtlas> a_fontAtlas, FontID a_id)
    {
      m_pimpl->DrawText(a_str, a_aabb, a_options, a_fontAtlas, a_id);
    }

    void Renderer::DrawBox(UIAABB const & a_aabb, Colour a_colour)
    {
      m_pimpl->DrawBox(a_aabb, a_colour);
    }

    //------------------------------------------------------------------------
    // Functions
    //------------------------------------------------------------------------

    bool Intersection(UIAABB const & A,
                      UIAABB const & B,
                      UIAABB & out)
    {
      float xMin = std::max(A.position.x(), B.position.x());
      float xMax = std::min(A.position.x() + A.size.x(), B.position.x() + B.size.x());

      if (xMin >= xMax)
        return false;

      float yMin = std::max(A.position.y(), B.position.y());
      float yMax = std::min(A.position.y() + A.size.y(), B.position.y() + B.size.y());

      if (yMin >= yMax)
        return false;

      out.position = vec2(xMin, yMin);
      out.size = vec2(xMax - xMin, yMax - yMin);

      return true;
    }

    bool PointInBox(vec2 const & a_point, UIAABB const & a_box)
    {
      if (a_point.x() < a_box.position.x())
        return false;

      if (a_point.y() < a_box.position.y())
        return false;

      if (a_point.x() > a_box.position.x() + a_box.size.x())
        return false;

      if (a_point.y() > a_box.position.y() + a_box.size.y())
        return false;

      return true;
    }
  }
}