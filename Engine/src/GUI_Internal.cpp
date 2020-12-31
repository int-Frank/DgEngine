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

#define DEFAULT_FONT_PATH "../Engine/assets/fonts/NotoSans-Regular.ttf"

namespace Engine
{
  namespace GUI
  {
    namespace Renderer
    {

      enum Mesh
      {
        uiM_Box,
        uiM_RoundedBox,

        uiM_COUNT
      };

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
      layout (location = 1) in vec2 inPosOffset_instance;
      layout (location = 1) in vec2 inTexOffset_instance;
      layout (location = 2) in vec2 inDim_instance;
      uniform vec2 windowSize;
      out vec2 texCoord;
      void main()
      {
        vec2 posXY = inPos * pixelSizes[gl_InstanceID] + posOffsets[gl_InstanceID];
        posXY = (posXY  / windowSize  - vec2(0.5, 0.5)) * 2.0;
        posXY.y = -posXY.y;
        texCoord = inPos * texOffsets[gl_InstanceID] + texOffsets[gl_InstanceID];
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
        ivec2 texDim = textureSize(textureAtlas, sampler, 0);
        vec2 texCoordn = texCoord / vec2(float(texDim.x), float(texDim.y));
        FragColor = vec4(textColour.x, textColour.y, textColour.z, textColour.w * texture(textureAtlas, texCoordn).x);
      })";

      static float const g_boxVerts[] =
      {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
      };

      static uint16_t const g_boxIndices[] ={0, 1, 2, 0, 2, 3};

      struct RenderContext
      {
        Ref<IFontAtlas> fontAtlas;
        FontID defaultFont;

        Ref<VertexBuffer> vb_box;
        Ref<IndexBuffer>  ib_box;
        Ref<VertexArray>  va_box;

        Ref<Material>     materialColourBox;
        //Ref<Material>     materialText;
      };

      static RenderContext *s_pRenderContext = nullptr;

      static void InitBoxVA()
      {
        s_pRenderContext->vb_box = VertexBuffer::Create(g_boxVerts, SIZEOF32(g_boxVerts));
        s_pRenderContext->vb_box->SetLayout(
          {
            { Engine::ShaderDataType::VEC2 }
          });

        s_pRenderContext->ib_box = Engine::IndexBuffer::Create(g_boxIndices, ARRAY_SIZE_32(g_boxIndices));
        s_pRenderContext->va_box = Engine::VertexArray::Create();

        s_pRenderContext->va_box->AddVertexBuffer(s_pRenderContext->vb_box);
        s_pRenderContext->va_box->SetIndexBuffer(s_pRenderContext->ib_box);
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

      //static void InitText()
      //{
      //  Engine::ShaderData * pSD = new ShaderData({
      //      { Engine::ShaderDomain::Vertex, Engine::StrType::Source, g_textShader_vs },
      //      { Engine::ShaderDomain::Fragment, Engine::StrType::Source, g_textShader_fs }
      //    });
      //
      //  ResourceManager::Instance()->RegisterResource(ir_GUITextShader, pSD);
      //  Ref<Engine::RendererProgram> refProg;
      //  refProg = Engine::RendererProgram::Create(ir_GUITextShader);
      //  s_pRenderContext->materialText = Material::Create(refProg);
      //}

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
        //InitText();

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
        //s_pRenderContext->materialText->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
      }

      void DrawBox(UIAABB const & a_aabb, Colour a_colour)
      {
        float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

        s_pRenderContext->materialColourBox->SetUniform("colour", clr, sizeof(clr));
        s_pRenderContext->materialColourBox->SetUniform("offset", a_aabb.position.GetData(), sizeof(a_aabb.position));
        s_pRenderContext->materialColourBox->SetUniform("scale", a_aabb.size.GetData(), sizeof(a_aabb.size));

        s_pRenderContext->materialColourBox->Bind();
        s_pRenderContext->va_box->Bind();

        ::Engine::Renderer::DrawIndexed(s_pRenderContext->va_box, RenderMode::Triangles, 1);
      }

      void DrawText(uint16_t a_textureID, Colour a_colour, uint32_t a_count, void * a_pVerts)
      {
        /*float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

        Ref<Texture2D> texture;
        if (s_pRenderContext->fontAtlas->GetTexture(a_textureID, texture) != Dg::ErrorCode::None)
          return;

        s_pRenderContext->renText.material->SetUniform("textColour ", clr, sizeof(clr));
        s_pRenderContext->renText.material->SetTexture("textureAtlas", texture);

        s_pRenderContext->renText.material->Bind();
        s_pRenderContext->renText.va->Bind();

        ::Engine::Renderer::DrawIndexed(a_count * 6, false);*/
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