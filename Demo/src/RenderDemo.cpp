
#include "common.h"
#include "RenderDemo.h"

#include "DgMath.h"
#include "Renderer.h"

using namespace DgE;

struct UBOData
{
  uint32_t uint_0;
};

static char const * g_vs = R"(
     #version 430
     layout (location = 0) in vec2 inPos;
     layout (location = 1) in vec2 inTexCoord;
     layout (location = 2) in vec2 inOffset;
     out vec2 texCoord;
     void main()
     {
         gl_Position = vec4(inPos + inOffset, 0.0, 1.0);
         texCoord = inTexCoord;
     })";

static char const * g_fs = R"(
      #version 430 core
      in vec2 texCoord;
      out vec4 FragColor;
      uniform sampler2D tex;
      void main()
      {
        FragColor = texture(tex, texCoord);
      })";

Colour * GenerateTexture(uint32_t dim)
{
  Colour * pPixels = new Colour[dim * dim];

  for (uint32_t y = 0; y < dim; y++)
  {
    for (uint32_t x = 0; x < dim; x++)
    {
      float p = 1.0f - (cos(float(x) / dim * 8.0f * Dg::Constants<float>::PI) / 2.0f + 0.5f);
      float q = 1.0f - (cos(float(y) / dim * 8.0f * Dg::Constants<float>::PI) / 2.0f + 0.5f);

      float r = p * q;
      float g = p * q * p * q;
      float b = p * q * p * q * p * q;

      pPixels[y * dim + x] = Colour(uint32_t(r * 255.0f), 
                                    uint32_t(g * 255.0f),
                                    uint32_t(b * 255.0f),
                                    255);
    }
  }

  return pPixels;
}

MAKE_SYSTEM_DEFINITION(RenderDemo)

void RenderDemo::OnAttach()
{
  uint32_t const dim = 256;

  float verts[] =
  {
    // position   // texture
    0.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 0.1f,   0.0f, 1.0f,
    0.1f, 0.1f,   1.0f, 1.0f,
    0.1f, 0.0f,   1.0f, 0.0f
  };

  float offsets[] =
  {
    -0.8f, -0.8f,
    -0.8f, -0.4f,
    -0.8f, -0.0f,
    -0.8f,  0.4f,
    -0.8f,  0.8f,

    -0.4f, -0.8f,
    -0.4f, -0.4f,
    -0.4f, -0.0f,
    -0.4f,  0.4f,
    -0.4f,  0.8f,

     0.0f, -0.8f,
     0.0f, -0.4f,
     0.0f, -0.0f,
     0.0f,  0.4f,
     0.0f,  0.8f,
     
     0.4f, -0.8f,
     0.4f, -0.4f,
     0.4f, -0.0f,
     0.4f,  0.4f,
     0.4f,  0.8f,
     
     0.8f, -0.8f,
     0.8f, -0.4f,
     0.8f, -0.0f,
     0.8f,  0.4f,
     0.8f,  0.8f,
  };

  uint16_t indices[] ={0, 1, 2, 0, 2, 3};

  {
    m_vb_box = VertexBuffer::Create(verts, SIZEOF32(verts), BF_None);
    m_vb_box->SetLayout(
      {
        { ShaderDataType::VEC2 }, // inPos
        { ShaderDataType::VEC2 }, // inTexCoord
      });

    m_vb_offsets = VertexBuffer::Create(offsets, SIZEOF32(offsets), BF_None);
    m_vb_offsets->SetLayout(
      {
        { ShaderDataType::VEC2 }, // inOffset
      });

    m_ib = IndexBuffer::Create(indices, ARRAY_SIZE_32(indices));
    m_va = VertexArray::Create();

    // Adding buffers to the vertex array must be in order as they appear in the shader.
    // Perhaps come up with a better way to link vertex attributes?
    m_va->AddVertexBuffer(m_vb_box);
    m_va->AddVertexBuffer(m_vb_offsets);
    m_va->SetIndexBuffer(m_ib);
    m_va->SetVertexAttributeDivisor(2, 1);

    ShaderData * pSD = new ShaderData({
      { ShaderDomain::Vertex, StrType::Source, g_vs },
      { ShaderDomain::Fragment, StrType::Source, g_fs }
      });

    Ref<RendererProgram> refProg;
    refProg = RendererProgram::Create(pSD);

    TextureAttributes attrs;
    attrs.SetFilter(TextureFilter::Linear);
    attrs.SetIsMipmapped(false);
    attrs.SetWrap(TextureWrap::Clamp);
    attrs.SetPixelType(TexturePixelType::RGBA8);
    m_texture = Texture2D::Create();
    m_texture->Set(dim, dim, GenerateTexture(dim), attrs);
    m_texture->Upload();

    m_material = Material::Create(refProg);
    m_material->SetTexture("tex", m_texture);

    // Set Uniform Buffer
    m_bindingPoint = BindingPoint::Create(StorageBlockType::Uniform, ShaderDomain::Fragment);
    m_ubo = UniformBuffer::Create((uint32_t)sizeof(UBOData));
    m_ubo->Bind(m_bindingPoint);
  }
}

void RenderDemo::Render()
{
  m_material->Bind();
  m_va->Bind();

  Renderer::DrawIndexed(m_va, DgE::RenderMode::Triangles, 25);
}