//@group Renderer

#include "Texture.h"
#include "RenderState.h"
#include "Renderer.h"
#include "RT_Texture.h"
#include "RenderThreadData.h"

namespace DgE
{
  Texture2D::Texture2D()
  {

  }

  Ref<Texture2D> Texture2D::Create()
  {
    return Ref<Texture2D>(new Texture2D());
  }

  void Texture2D::Set(uint32_t a_width, uint32_t a_height, void * a_pPixels, TextureAttributes a_attrs)
  {
    m_data.Set(a_width, a_height, a_pPixels, a_attrs);
  }

  Texture2D::~Texture2D()
  {
    m_data.Clear();

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::TextureDelete);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_Texture2D ** ppTexture =  RenderThreadData::Instance()->textures.at(resID);
      if (ppTexture == nullptr)
        return;

      delete *ppTexture;
      *ppTexture = nullptr;
      RenderThreadData::Instance()->textures.erase(resID);
    });
  }

  void Texture2D::Upload(bool freePixels)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::TextureCreate);

    TextureData * pData = new TextureData();
    pData->Duplicate(m_data);
    
    if (freePixels)
    {
      delete[] m_data.pPixels;
      m_data.pPixels = nullptr;
    }

    RENDER_SUBMIT(state, [resID = m_id, pData = pData]() mutable
    {
      // TODO all of these we should check that *ptr != nullptr, but really, nullptrs should not be in RenderThreadData
      RT_Texture2D ** ppTexture = RenderThreadData::Instance()->textures.at(resID);
      if (ppTexture != nullptr)
      {
        delete *ppTexture;
        *ppTexture = nullptr;
        RenderThreadData::Instance()->textures.erase(resID);
      }
        
      RenderThreadData::Instance()->textures.insert(resID, RT_Texture2D::Create(*pData));
      delete pData;
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

    RENDER_SUBMIT(state, [resID = m_id, slot = a_slot]()
    {
      RT_Texture2D ** ppTexture =  RenderThreadData::Instance()->textures.at(resID);
      if (ppTexture == nullptr)
      {
        LOG_WARN("Texture2D::Bind(): ID '{}' does not exist!", resID);
        return;
      }

      (*ppTexture)->Bind(slot);
    });
  }
}