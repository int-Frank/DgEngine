#ifndef RENDERTEXTUREDEMO_H
#define RENDERTEXTUREDEMO_H

#include "common.h"
#include "System.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RendererProgram.h"
#include "Material.h"
#include "Texture.h"

class RenderDemo : public Engine::System
{
public:

  MAKE_SYSTEM_DECL(SID_RenderDemo)

  void OnAttach() override;
  void HandleMessage(Engine::Message * a_pMsg) override {}
  void OnDetach() override {}
  void Render() override;
  void Update(float a_dt) override {}

private:

  Engine::Ref<Engine::VertexBuffer>   m_vb_box;
  Engine::Ref<Engine::VertexBuffer>   m_vb_offsets;
  Engine::Ref<Engine::UniformBuffer>  m_ubo;

  Engine::Ref<Engine::IndexBuffer>  m_ib;
  Engine::Ref<Engine::VertexArray>  m_va;
  Engine::Ref<Engine::Texture2D>    m_texture;
  Engine::Ref<Engine::Material>     m_material;
  Engine::Ref<Engine::BindingPoint> m_bindingPoint;
};

#endif
