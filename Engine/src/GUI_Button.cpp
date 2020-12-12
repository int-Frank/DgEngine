//@group UI

#include "GUI_Button.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "Renderer.h"

namespace Engine
{
  namespace GUI
  {

    Button::Button(Widget * a_pParent, std::string const & a_text, uint32_t a_fontID, vec2 const & a_position, vec2 const & a_size)
      : m_text(a_text)
      , m_fontID(a_fontID)
      , m_clrDefault(0xFF0000FF)
      , m_clrHover(0xFF00FFFF)
      , m_aabb{a_position, a_size}
      , m_state(WidgetState::None)
      , m_pParent(a_pParent)
      , m_clbk_HoverOn(nullptr)
      , m_clbk_HoverOff(nullptr)
      , m_clbk_Select(nullptr)
    {

    }

    Button::~Button()
    {

    }

    Button * Button::Create(Widget * pParent, std::string const & text, uint32_t fontID, vec2 const & position, vec2 const & size)
    {
      return new Button(pParent, text, fontID, position, size);
    }

    void Button::BindHoverOn(std::function<void()> a_fn)
    {
      m_clbk_HoverOn = a_fn;
    }

    void Button::BindHoverOff(std::function<void()> a_fn)
    {
      m_clbk_HoverOff = a_fn;
    }

    void Button::BindSelect(std::function<void()> a_fn)
    {
      m_clbk_Select = a_fn;
    }

    void Button::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      DISPATCH_MESSAGE(Message_GUI_PointerMove);
      DISPATCH_MESSAGE(Message_GUI_PointerDown);
    }

    void Button::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (GetGlobalAABB(aabb) == AABBType::None)
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && m_clbk_Select != nullptr)
      {
        m_clbk_Select();
        a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
      }
    }

    void Button::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      UIAABB aabb;
      if (GetGlobalAABB(aabb) == AABBType::None)
        return;

      bool isInside = PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb);
      if (isInside)
        a_pMsg->ConsumeHover();

      if (isInside && m_state == WidgetState::None)
      {
        m_state = WidgetState::HoverOn;
        if (m_clbk_HoverOn != nullptr)
          m_clbk_HoverOn();
      }
      if (!isInside && m_state == WidgetState::HoverOn)
      {
        m_state = WidgetState::None;
        if (m_clbk_HoverOff != nullptr)
          m_clbk_HoverOff();
      }
    }

    void Button::ClearBindings()
    {
      m_clbk_HoverOn = nullptr;
      m_clbk_HoverOff = nullptr;
      m_clbk_Select = nullptr;
    }

    void Button::Draw()
    {
      UIAABB sissor;
      AABBType t = GetGlobalAABB(sissor);
      if (t == AABBType::None)
        return;

      if (t == AABBType::Window)
      {
        ::Engine::Renderer::Enable(RenderFeature::Sissor);
        ::Engine::Renderer::SetSissorBox((int)sissor.position.x(), (int)sissor.position.y(), (int)sissor.size.x(), (int)sissor.size.y());
        Renderer::Instance()->DrawBox({GetGlobalPosition(), GetSize()}, m_state == WidgetState::None ? m_clrDefault : m_clrHover);
        ::Engine::Renderer::Disable(RenderFeature::Sissor);
      }
      else
      {
        Renderer::Instance()->DrawBox(m_aabb, m_state == WidgetState::None ? m_clrDefault : m_clrHover);
      }
    }

    WidgetState Button::QueryState() const
    {
      return m_state;
    }

    Widget * Button::GetParent() const
    {
      return m_pParent;
    }

    void Button::SetParent(Widget * a_pParent)
    {
      m_pParent = a_pParent;
    }

    vec2 Button::GetLocalPosition() const
    {
      return m_aabb.position;
    }

    vec2 Button::GetSize() const
    {
      return m_aabb.size;
    }

    void Button::SetPosition(vec2 const & a_position)
    {
      m_aabb.position = a_position;
    }

    void Button::SetSize(vec2 const & a_size)
    {
      m_aabb.size = a_size;
    }

    void Button::SetBackgroundColour(Colour a_clr)
    {
      m_clrDefault = a_clr;
    }

    void Button::SetHoverOnBackgroundColour(Colour a_clr)
    {
      m_clrHover = a_clr;
    }
  }
}