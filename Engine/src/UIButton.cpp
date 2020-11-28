//@group UI

#include "UIButton.h"
#include "MessageHandler.h"
#include "UI_Internal.h"
#include "Renderer.h"

namespace Engine
{
  UIButton::UIButton(UIWidget * a_pParent, std::string const & a_text, uint32_t a_fontID, vec2 const & a_position, vec2 const & a_size)
    : m_text(a_text)
    , m_fontID(a_fontID)
    , m_clrDefault(0xFF0000FF)
    , m_clrHover(0xFF00FFFF)
    , m_aabb{a_position, a_size}
    , m_state(UIState::None)
    , m_pParent(a_pParent)
    , m_clbk_HoverOn(nullptr)
    , m_clbk_HoverOff(nullptr)
    , m_clbk_Select(nullptr)
  {

  }

  UIButton::~UIButton()
  {

  }

  UIButton * UIButton::Create(UIWidget * pParent, std::string const & text, uint32_t fontID, vec2 const & position, vec2 const & size)
  {
    return new UIButton(pParent, text, fontID, position, size);
  }

  void UIButton::BindHoverOn(std::function<void()> a_fn)
  {
    m_clbk_HoverOn = a_fn;
  }

  void UIButton::BindHoverOff(std::function<void()> a_fn)
  {
    m_clbk_HoverOff = a_fn;
  }

  void UIButton::BindSelect(std::function<void()> a_fn)
  {
    m_clbk_Select = a_fn;
  }

  void UIButton::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    DISPATCH_MESSAGE(Message_GUI_PointerMove);
    DISPATCH_MESSAGE(Message_GUI_PointerDown);
  }

  void UIButton::HandleMessage(Message_GUI_PointerDown * a_pMsg)
  {
    UIAABB aabb;
    if (GetGlobalAABB(aabb) == UIAABBType::None)
      return;

    if (UIPointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && m_clbk_Select != nullptr)
    {
      m_clbk_Select();
      a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
    }
  }

  void UIButton::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    UIAABB aabb;
    if (GetGlobalAABB(aabb) == UIAABBType::None)
      return;

    bool isInside = UIPointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb);
    if (isInside && m_state == UIState::None)
    {
      m_state = UIState::HoverOn;
      if (m_clbk_HoverOn != nullptr)
        m_clbk_HoverOn();
    }
    if (!isInside && m_state == UIState::HoverOn)
    {
      m_state = UIState::None;
      if (m_clbk_HoverOff != nullptr)
        m_clbk_HoverOff();
    }
  }

  void UIButton::ClearBindings()
  {
    m_clbk_HoverOn = nullptr;
    m_clbk_HoverOff = nullptr;
    m_clbk_Select = nullptr;
  }

  void UIButton::Draw()
  {
    UIAABB sissor;
    UIAABBType t = GetGlobalAABB(sissor);
    if (t == UIAABBType::None)
      return;

    //UIAABB aabb = {GetGlobalPosition(), GetSize()};

    if (t == UIAABBType::Window)
    {
      Renderer::Enable(RenderFeature::Sissor);
      Renderer::SetSissorBox((int)sissor.position.x(), (int)sissor.position.y(), (int)sissor.size.x(), (int)sissor.size.y());
      UIRenderer::Instance()->DrawBox({GetGlobalPosition(), GetSize()}, m_state == UIState::None ? m_clrDefault : m_clrHover);
      Renderer::Disable(RenderFeature::Sissor);
    }
    else
    {
      UIRenderer::Instance()->DrawBox(m_aabb, m_state == UIState::None ? m_clrDefault : m_clrHover);
    }
  }

  UIState UIButton::QueryState() const
  {
    return m_state;
  }

  UIWidget * UIButton::GetParent() const
  {
    return m_pParent;
  }

  void UIButton::SetParent(UIWidget * a_pParent)
  {
    m_pParent = a_pParent;
  }

  vec2 UIButton::GetLocalPosition() const
  {
    return m_aabb.position;
  }

  vec2 UIButton::GetSize() const
  {
    return m_aabb.size;
  }

  void UIButton::SetPosition(vec2 const & a_position)
  {
    m_aabb.position = a_position;
  }

  void UIButton::SetSize(vec2 const & a_size)
  {
    m_aabb.size = a_size;
  }

  void UIButton::SetBackgroundColour(Colour a_clr)
  {
    m_clrDefault = a_clr;
  }

  void UIButton::SetHoverOnBackgroundColour(Colour a_clr)
  {
    m_clrHover = a_clr;
  }
}