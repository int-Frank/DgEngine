//@group GUI

#include "GUI_Internal.h"

#include "BSR_Assert.h"
#include "Framework.h"
#include "Material.h"
#include "ShaderUniform.h"
#include "RendererProgram.h"
#include "VertexArray.h"
#include "Renderer.h"
#include <algorithm>

#define DEFAULT_FONT_PATH "../Engine/assets/fonts/NotoSans-BSR.ttf"

namespace Engine
{
  namespace GUI
  {
    namespace Renderer
    {
      static char const * g_flatShader_vs = R"(
      #version 430
      layout(location = 0) in vec2 inPos;
      uniform vec2 windowSize;
      uniform vec2 offset;
      uniform vec2 scale;
      void main()
      {
        vec2 xy = ((inPos * scale + offset)  / windowSize  - vec2(0.5, 0.5)) * 2.0;
        xy.y = -xy.y;
        gl_Position = vec4(xy, 0.0, 1.0);
      })";

      static char const * g_flatShader_fs = R"(
      #version 430
      uniform vec4 colour;
      out vec4 FragColour;
      void main()
      {
        FragColour = colour;
      })";

      static char const * g_textShader_vs = R"(
      #version 430
      layout (location = 0) in vec2 inPos;
      layout (location = 1) in vec2 inPosOffset;
      layout (location = 2) in vec2 inTexOffset;
      layout (location = 3) in vec2 inScale;
      uniform vec2 windowSize;
      out vec2 texCoord;
      void main()
      {
        vec2 posXY = ((inPos * inScale + inPosOffset)  / windowSize  - vec2(0.5, 0.5)) * 2.0;
        posXY.y = -posXY.y;
        texCoord = inPos * inScale + inTexOffset;
        gl_Position = vec4(posXY, 0.0, 1.0);
      })";

      static char const * g_textShader_fs = R"(
      #version 430
      in vec2 texCoord;
      out vec4 FragColor;
      uniform sampler2D textureAtlas;
      uniform vec4 textColour;
      void main()
      {
        ivec2 texDim = textureSize(textureAtlas, 0);
        vec2 texCoordn = texCoord / vec2(float(texDim.x), float(texDim.y));
        FragColor = vec4(textColour.x, textColour.y, textColour.z, textColour.w * texture(textureAtlas, texCoordn).x);
      })";

      static float const g_unitBoxVerts[] =
      {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
      };

      static uint16_t const g_unitBoxIndices[] ={0, 1, 2, 0, 2, 3};

      struct RenderContext
      {
        Ref<IFontAtlas> fontAtlas;
        FontID defaultFont;

        Ref<VertexBuffer> vb_unitBoxPos;
        Ref<IndexBuffer>  ib_unitBox;
        Ref<VertexArray>  va_unitBox;

        Ref<Material>     materialColourBox;

        Ref<VertexBuffer> vb_textInstance;
        Ref<VertexArray>  va_text;
        Ref<Material>     materialText;
      };

      static RenderContext *s_pRenderContext = nullptr;

      static void InitBoxVA()
      {
        s_pRenderContext->vb_unitBoxPos = VertexBuffer::Create(g_unitBoxVerts, SIZEOF32(g_unitBoxVerts));
        s_pRenderContext->vb_unitBoxPos->SetLayout(
          {
            { Engine::ShaderDataType::VEC2 }
          });

        s_pRenderContext->ib_unitBox = Engine::IndexBuffer::Create(g_unitBoxIndices, ARRAY_SIZE_32(g_unitBoxIndices));
        s_pRenderContext->va_unitBox = Engine::VertexArray::Create();

        s_pRenderContext->va_unitBox->AddVertexBuffer(s_pRenderContext->vb_unitBoxPos);
        s_pRenderContext->va_unitBox->SetIndexBuffer(s_pRenderContext->ib_unitBox);
      }

      static void InitBox()
      {
        Engine::ShaderData * pSD = new ShaderData({
            { Engine::ShaderDomain::Vertex, Engine::StrType::Source, g_flatShader_vs },
            { Engine::ShaderDomain::Fragment, Engine::StrType::Source, g_flatShader_fs }
          });

        ResourceManager::Instance()->RegisterResource(ir_GUIBoxShader, pSD);
        Ref<Engine::RendererProgram> refProg;
        refProg = Engine::RendererProgram::Create(ir_GUIBoxShader);
        s_pRenderContext->materialColourBox = Material::Create(refProg);
      }

      static void InitText()
      {
        s_pRenderContext->vb_textInstance = VertexBuffer::Create(MAX_TEXT_CHARACTERS * sizeof(float) * 6);
        s_pRenderContext->vb_textInstance->SetLayout(
          {
            { Engine::ShaderDataType::VEC2 }, // inPosOffset
            { Engine::ShaderDataType::VEC2 }, // inTexOffset
            { Engine::ShaderDataType::VEC2 }  // inScale
          });

        s_pRenderContext->va_text = Engine::VertexArray::Create();

        s_pRenderContext->va_text->AddVertexBuffer(s_pRenderContext->vb_unitBoxPos);
        s_pRenderContext->va_text->AddVertexBuffer(s_pRenderContext->vb_textInstance);
        s_pRenderContext->va_text->SetIndexBuffer(s_pRenderContext->ib_unitBox);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(1, 1);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(2, 1);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(3, 1);

        Engine::ShaderData * pSD = new ShaderData({
            { Engine::ShaderDomain::Vertex, Engine::StrType::Source, g_textShader_vs },
            { Engine::ShaderDomain::Fragment, Engine::StrType::Source, g_textShader_fs }
          });
      
        ResourceManager::Instance()->RegisterResource(ir_GUITextShader, pSD);
        Ref<Engine::RendererProgram> refProg;
        refProg = Engine::RendererProgram::Create(ir_GUITextShader);
        s_pRenderContext->materialText = Material::Create(refProg);
      }

      Dg::ErrorCode Init()
      {
        Dg::ErrorCode result;

        DG_ERROR_IF(s_pRenderContext != nullptr, Dg::ErrorCode::Disallowed);
        s_pRenderContext = new RenderContext();

        s_pRenderContext->defaultFont = 0;
        s_pRenderContext->fontAtlas = Framework::Instance()->CreateFontAtlas();

        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterFont(DEFAULT_FONT_PATH, s_pRenderContext->defaultFont));
        s_pRenderContext->fontAtlas->SetTextureDimension(FONTATLAS_DEFAULT_TEXTURE_DIMENSION);
        s_pRenderContext->fontAtlas->BeginLoad();
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterAllGlyphs(s_pRenderContext->defaultFont, DEFAULT_FONT_SIZE));
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->CommitLoad());

        InitBoxVA();
        InitBox();
        InitText();

        result = Dg::ErrorCode::None;
      epilogue:
        return result;
      }

      void Destroy()
      {
        delete s_pRenderContext;
        s_pRenderContext = nullptr;
      }

      void SetScreenSize(vec2 const & a_size)
      {
        s_pRenderContext->materialColourBox->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
        s_pRenderContext->materialText->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
      }

      void DrawBox(UIAABB const & a_aabb, Colour a_colour)
      {
        float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

        s_pRenderContext->materialColourBox->SetUniform("colour", clr, sizeof(clr));
        s_pRenderContext->materialColourBox->SetUniform("offset", a_aabb.position.GetData(), sizeof(a_aabb.position));
        s_pRenderContext->materialColourBox->SetUniform("scale", a_aabb.size.GetData(), sizeof(a_aabb.size));

        s_pRenderContext->materialColourBox->Bind();
        s_pRenderContext->va_unitBox->Bind();

        ::Engine::Renderer::DrawIndexed(s_pRenderContext->va_unitBox, RenderMode::Triangles, 1);
      }

      void DrawText(uint16_t a_textureID, Colour a_colour, uint32_t a_count, void * a_pVerts)
      {
        if (a_count == 0 || a_pVerts == nullptr)
          return;

        float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

        Ref<Texture2D> texture;
        if (s_pRenderContext->fontAtlas->GetTexture(a_textureID, texture) != Dg::ErrorCode::None)
          return;

        s_pRenderContext->vb_textInstance->SetData(a_pVerts, a_count * sizeof(float) * 6, 0);

        s_pRenderContext->materialText->SetUniform("textColour", clr, sizeof(clr));
        s_pRenderContext->materialText->SetTexture("textureAtlas", texture);

        s_pRenderContext->materialText->Bind();
        s_pRenderContext->va_text->Bind();

        ::Engine::Renderer::DrawIndexed(s_pRenderContext->va_text, RenderMode::Triangles, a_count);
      }

      GlyphData * GetGlyphData(CodePoint a_cp)
      {
        return s_pRenderContext->fontAtlas->GetGlyphData(s_pRenderContext->defaultFont, a_cp, DEFAULT_FONT_SIZE);
      }

      void GetCharacterSizeRange(int16_t & a_ascent, int16_t & a_descent)
      {
        a_ascent = 0;
        a_descent = 0;
        Dg::ErrorCode result = s_pRenderContext->fontAtlas->GetCharacterSizeRange(s_pRenderContext->defaultFont, a_ascent, a_descent);
        BSR_ASSERT(result == Dg::ErrorCode::None, "Default font malformed!");
      }
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