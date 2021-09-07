#ifndef RENDERTEXTUREDEMO_H
#define RENDERTEXTUREDEMO_H

#include "common.h"
#include "System.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RendererProgram.h"
#include "Material.h"
#include "Texture.h"

class RenderDemo : public DgE::System
{
public:

  MAKE_SYSTEM_DECL

  void OnAttach() override;
  void HandleMessage(DgE::Message * a_pMsg) override {}
  void OnDetach() override {}
  void Render() override;
  void Update(float a_dt) override {}

private:

  DgE::Ref<DgE::VertexBuffer>   m_vb_box;
  DgE::Ref<DgE::VertexBuffer>   m_vb_offsets;
  DgE::Ref<DgE::UniformBuffer>  m_ubo;

  DgE::Ref<DgE::IndexBuffer>  m_ib;
  DgE::Ref<DgE::VertexArray>  m_va;
  DgE::Ref<DgE::Texture2D>    m_texture;
  DgE::Ref<DgE::Material>     m_material;
  DgE::Ref<DgE::BindingPoint> m_bindingPoint;
};

#endif
