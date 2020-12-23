//@group GUI

#include "GUI_Text.h"
#include "Options.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "Renderer.h"

namespace Engine
{
  namespace GUI
  {
    Text::Text(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags)
      : Widget(flags)
      , m_pParent(pParent)
      , m_text(text)
      , m_attributes{}
      , m_aabb{position, size}
    {
      if (pAttrs != nullptr)
      {
        m_attributes = *pAttrs;
      }
      else
      {
        // TODO these need to come from settings
        m_attributes.size = DEFAULT_FONT_SIZE; // TODO this needs to be the same as whatever the font atals loaded.
        m_attributes.colourText = 0xFFFFFFFF;
        m_attributes.colourBackground = 0x99999999;
        m_attributes.horizontalAlign = TextAlignment::Min;
        m_attributes.verticalAlign = TextAlignment::Min;
        m_attributes.wrapType = TextWrapType::Word;
      }
    }

    Text * Text::Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags)

    {
      return new Text(pParent, text, position, size, pAttrs, flags);
    }

    Text::~Text()
    {

    }

    void Text::SetBorder(float a_border)
    {
      m_border = a_border;
    }

    void Text::SetText(std::string const & a_str)
    {
      m_text = a_str;
    }

    void Text::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalAABB(viewableWindow))
        return;

      ::Engine::Renderer::Enable(RenderFeature::Sissor);
      ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      Renderer::Instance()->DrawBox({GetGlobalPosition(), GetSize()}, m_attributes.colourBackground);
      ::Engine::Renderer::Disable(RenderFeature::Sissor);
    }
    
    WidgetState Text::QueryState() const
    {
      return WidgetState::None;
    }

    Widget * Text::GetParent() const
    {
      return m_pParent;
    }

    void Text::SetParent(Widget * a_pParent)
    {
      m_pParent = a_pParent;
    }

    void Text::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      DISPATCH_MESSAGE(Message_GUI_PointerMove);
      DISPATCH_MESSAGE(Message_GUI_PointerDown);
    }

    void Text::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
    }

    void Text::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->ConsumeHover();
    }

    void Text::_SetLocalPosition(vec2 const & a_pos)
    {
      m_aabb.position = a_pos;
    }

    void Text::_SetSize(vec2 const & a_size)
    {
      m_aabb.size = a_size;
    }

    vec2 Text::_GetLocalPosition()
    {
      return m_aabb.position;
    }

    vec2 Text::_GetSize()
    {
      return m_aabb.size;
    }
  }
}