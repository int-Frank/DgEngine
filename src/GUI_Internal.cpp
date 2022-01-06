//@group GUI

#include "GUI.h"
#include "GUI_Internal.h"
#include "Options.h"
#include "Framework.h"
#include "Material.h"
#include "ShaderUniform.h"
#include "RendererProgram.h"
#include "VertexArray.h"
#include "Renderer.h"
#include <algorithm>

// TODO This needs to come from input args or something
#define DEFAULT_FONT_PATH "../DgEngine/assets/fonts/NotoSans-BSR.ttf"

namespace DgE
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

      static char const * g_boxBorderShader_vs = R"(
      #version 430
      layout(location = 0) in vec2 inPos;
      uniform vec2 windowSize;
      void main()
      {
        vec2 xy = (inPos  / windowSize  - vec2(0.5, 0.5)) * 2.0;
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

      static uint16_t const g_boxBorderIndices[] =
      {
        0, 5, 4,  0, 5, 1,
        1, 6, 5,  1, 6, 2,
        2, 7, 6,  2, 7, 3,
        3, 4, 7,  3, 4, 0
      };

      struct RenderContext
      {
        Ref<IFontAtlas> fontAtlas;
        FontID defaultFont;

        Ref<VertexBuffer> vb_unitBox;
        Ref<IndexBuffer>  ib_unitBox;
        Ref<VertexArray>  va_unitBox;
        Ref<Material>     materialColourBox;

        Ref<VertexBuffer> vb_boxBorder;
        Ref<IndexBuffer>  ib_boxBorder;
        Ref<VertexArray>  va_boxBorder;
        Ref<Material>     materialBoxBorder;

        Ref<VertexBuffer> vb_textInstance;
        Ref<VertexArray>  va_text;
        Ref<Material>     materialText;
      };

      static RenderContext *s_pRenderContext = nullptr;

      static void InitBox()
      {
        s_pRenderContext->vb_unitBox = VertexBuffer::Create(g_unitBoxVerts, SIZEOF32(g_unitBoxVerts), BF_None);
        s_pRenderContext->vb_unitBox->SetLayout(
          {
            { DgE::ShaderDataType::VEC2 }
          });

        s_pRenderContext->ib_unitBox = DgE::IndexBuffer::Create(g_unitBoxIndices, ARRAY_SIZE_32(g_unitBoxIndices));
        s_pRenderContext->va_unitBox = DgE::VertexArray::Create();

        s_pRenderContext->va_unitBox->AddVertexBuffer(s_pRenderContext->vb_unitBox);
        s_pRenderContext->va_unitBox->SetIndexBuffer(s_pRenderContext->ib_unitBox);

        DgE::ShaderData * pSD = new ShaderData({
            { DgE::ShaderDomain::Vertex, DgE::StrType::Source, g_flatShader_vs },
            { DgE::ShaderDomain::Fragment, DgE::StrType::Source, g_flatShader_fs }
          });

        ResourceManager::Instance()->RegisterResource(ir_GUIBoxShader, pSD);
        Ref<DgE::RendererProgram> refProg;
        refProg = DgE::RendererProgram::Create(ir_GUIBoxShader);
        s_pRenderContext->materialColourBox = Material::Create(refProg);
      }

      static void InitBoxBorder()
      {
        s_pRenderContext->vb_boxBorder = VertexBuffer::Create(sizeof(float) * 2 * 8, BF_None);
        s_pRenderContext->vb_boxBorder->SetLayout(
          {
            { DgE::ShaderDataType::VEC2 }
          });

        s_pRenderContext->ib_boxBorder = DgE::IndexBuffer::Create(g_boxBorderIndices, ARRAY_SIZE_32(g_boxBorderIndices));
        s_pRenderContext->va_boxBorder = DgE::VertexArray::Create();

        s_pRenderContext->va_boxBorder->AddVertexBuffer(s_pRenderContext->vb_boxBorder);
        s_pRenderContext->va_boxBorder->SetIndexBuffer(s_pRenderContext->ib_boxBorder);

        DgE::ShaderData * pSD = new ShaderData({
            { DgE::ShaderDomain::Vertex, DgE::StrType::Source, g_boxBorderShader_vs },
            { DgE::ShaderDomain::Fragment, DgE::StrType::Source, g_flatShader_fs }
          });

        ResourceManager::Instance()->RegisterResource(ir_GUIBoxBorderShader, pSD);
        Ref<DgE::RendererProgram> refProg;
        refProg = DgE::RendererProgram::Create(ir_GUIBoxBorderShader);
        s_pRenderContext->materialBoxBorder = Material::Create(refProg);
      }

      static void InitText()
      {
        s_pRenderContext->vb_textInstance = VertexBuffer::Create(MAX_TEXT_CHARACTERS * sizeof(float) * 6, BF_None);
        s_pRenderContext->vb_textInstance->SetLayout(
          {
            { DgE::ShaderDataType::VEC2 }, // inPosOffset
            { DgE::ShaderDataType::VEC2 }, // inTexOffset
            { DgE::ShaderDataType::VEC2 }  // inScale
          });

        s_pRenderContext->va_text = DgE::VertexArray::Create();

        s_pRenderContext->va_text->AddVertexBuffer(s_pRenderContext->vb_unitBox);
        s_pRenderContext->va_text->AddVertexBuffer(s_pRenderContext->vb_textInstance);
        s_pRenderContext->va_text->SetIndexBuffer(s_pRenderContext->ib_unitBox);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(1, 1);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(2, 1);
        s_pRenderContext->va_text->SetVertexAttributeDivisor(3, 1);

        DgE::ShaderData * pSD = new ShaderData({
            { DgE::ShaderDomain::Vertex, DgE::StrType::Source, g_textShader_vs },
            { DgE::ShaderDomain::Fragment, DgE::StrType::Source, g_textShader_fs }
          });
      
        ResourceManager::Instance()->RegisterResource(ir_GUITextShader, pSD);
        Ref<DgE::RendererProgram> refProg;
        refProg = DgE::RendererProgram::Create(ir_GUITextShader);
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

        // TODO allow the user to load other fonts and font sizes
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterAllGlyphs(s_pRenderContext->defaultFont, GUI_FONT_SIZE));
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterGlyph(s_pRenderContext->defaultFont, GUI_FONT_SIZE_GRAB,  0xE000));
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterGlyph(s_pRenderContext->defaultFont, GUI_FONT_SIZE_RADIO, 0x25C9)); // Checked radio
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterGlyph(s_pRenderContext->defaultFont, GUI_FONT_SIZE_RADIO, 0x25CB)); // Radio
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterGlyph(s_pRenderContext->defaultFont, GUI_FONT_SIZE_BOX,   0x2610));
        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->RegisterGlyph(s_pRenderContext->defaultFont, GUI_FONT_SIZE_TICK,  0x2714));

        DG_ERROR_CHECK(s_pRenderContext->fontAtlas->CommitLoad());

        InitBox();
        InitBoxBorder();
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
        s_pRenderContext->materialBoxBorder->SetUniform("windowSize", a_size.GetData(), sizeof(a_size));
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

        ::DgE::Renderer::DrawIndexed(s_pRenderContext->va_unitBox, RenderMode::Triangles, 1);
      }

      void DrawBoxBorder(UIAABB const & a_inner, float a_thickness, Colour a_colour)
      {
        float verts[16] = 
        {
          a_inner.position.x(),                    a_inner.position.y(),
          a_inner.position.x() + a_inner.size.x(), a_inner.position.y(),
          a_inner.position.x() + a_inner.size.x(), a_inner.position.y() + a_inner.size.y(),
          a_inner.position.x(),                    a_inner.position.y() + a_inner.size.y(),

          a_inner.position.x() - a_thickness,                    a_inner.position.y() - a_thickness,
          a_inner.position.x() + a_inner.size.x() + a_thickness, a_inner.position.y() - a_thickness,
          a_inner.position.x() + a_inner.size.x() + a_thickness, a_inner.position.y() + a_inner.size.y() + a_thickness,
          a_inner.position.x() - a_thickness,                    a_inner.position.y() + a_inner.size.y() + a_thickness,
        };

        float clr[4] ={a_colour.fr(), a_colour.fg(), a_colour.fb(), a_colour.fa()};

        s_pRenderContext->vb_boxBorder->SetData(verts, sizeof(float) * 16, 0);

        s_pRenderContext->materialBoxBorder->SetUniform("colour", clr, sizeof(clr));

        s_pRenderContext->materialBoxBorder->Bind();
        s_pRenderContext->va_boxBorder->Bind();

        ::DgE::Renderer::DrawIndexed(s_pRenderContext->va_boxBorder, RenderMode::Triangles, 1);
      }

      void DrawBoxWithBorder(UIAABB const & inner, float thickness, Colour clrInner, Colour clrBorder)
      {
        DrawBox(inner, clrInner);
        DrawBoxBorder(inner, thickness, clrBorder);
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

        ::DgE::Renderer::DrawIndexed(s_pRenderContext->va_text, RenderMode::Triangles, a_count);
      }

      Dg::ErrorCode DrawGlyph(CodePoint cp, uint32_t size, vec2 const &position, Colour colour)
      {
        Dg::ErrorCode result;
        float verts[6] = {};
        GlyphData * pGlyph = s_pRenderContext->fontAtlas->GetGlyphData(s_pRenderContext->defaultFont, cp, size);

        DG_ERROR_IF(pGlyph == nullptr, Dg::ErrorCode::NotFound);

        verts[0] = position.x();
        verts[1] = position.y();
        verts[2] = pGlyph->posX;
        verts[3] = pGlyph->posY;
        verts[4] = pGlyph->width;
        verts[5] = pGlyph->height;

        DrawText(pGlyph->textureID, colour, 1, verts);

        result = Dg::ErrorCode::None;
      epilogue:
        return result;
      }

      GlyphData * GetGlyphData(CodePoint a_cp, uint32_t a_size)
      {
        return s_pRenderContext->fontAtlas->GetGlyphData(s_pRenderContext->defaultFont, a_cp, a_size);
      }

      void GetCharacterSizeRange(uint32_t a_size, int16_t & a_ascent, int16_t & a_descent)
      {
        a_ascent = 0;
        a_descent = 0;
        Dg::ErrorCode result = s_pRenderContext->fontAtlas->GetCharacterSizeRange(s_pRenderContext->defaultFont, a_size, a_ascent, a_descent);
        DG_ASSERT(result == Dg::ErrorCode::None, "No glyphs for size!");
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