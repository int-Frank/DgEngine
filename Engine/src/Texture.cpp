//@group Renderer

#include "Texture.h"
#include "RenderState.h"
#include "Renderer.h"
#include "RT_Texture.h"
#include "RenderThreadData.h"

namespace Engine
{
  Texture2D::Texture2D()
  {

  }

  Ref<Texture2D> Texture2D::Create()
  {
    return Ref<Texture2D>(new Texture2D());
  }

  void Texture2D::Set(uint32_t a_width, uint32_t a_height, RGBA * a_pPixels, TextureFlags a_flags)
  {
    m_data.Set(a_width, a_height, a_pPixels, a_flags);
  }

  Texture2D::~Texture2D()
  {
    m_data.Clear();

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::TextureDelete);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]()
    {
      RT_Texture2D* pTexture =  RenderThreadData::Instance()->textures.at(resID);
      if (pTexture == nullptr)
        return;

      pTexture->Destroy();
      RenderThreadData::Instance()->textures.erase(resID);
    });
  }

  void Texture2D::Upload()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::TextureCreate);

    TextureData data;
    data.Duplicate(m_data);
    
    RENDER_SUBMIT(state, [resID = GetRefID().GetID(), data = data]() mutable
    {
      RT_Texture2D *pTexture =  RenderThreadData::Instance()->textures.at(resID);
      if (pTexture != nullptr)
        pTexture->Destroy();
      else
        pTexture = RenderThreadData::Instance()->textures.insert(resID, RT_Texture2D());
      
      pTexture->Init(data);
      data.Clear();
    });
  }

  void Texture2D::Clear()
  {
    m_data.Clear();
  }

  void Texture2D::Bind(uint32_t a_slot) const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::TextureBindToSlot);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID(), slot = a_slot]()
    {
      RT_Texture2D* pTexture =  RenderThreadData::Instance()->textures.at(resID);
      if (pTexture != nullptr)
      {
        LOG_WARN("Texture2D::Bind(): ID '{}' does not exist!", resID);
        return;
      }

      pTexture->Bind(slot);
    });
  }
}