//@group UI

#include "UIButton.h"
#include "MessageHandler.h"
#include "UI_Internal.h"

namespace Engine
{
  UIButton::UIButton(UIWidget * a_pParent, std::string const & a_text, uint32_t a_fontID, vec2 const & a_position, vec2 const & a_size)
    : m_text(a_text)
    , m_fontID(a_fontID)
    , m_position(a_position)
    , m_size(a_size)
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

  void UIButton::BindHoverOn(std::function<void()> a_fn)
  {
    m_clbk_HoverOn = a_fn;
  }

  void UIButton::BindHoverOff(std::function<void()> a_fn)
  {
    m_clbk_HoverOff = a_fn;
  }

  void UIButton::BindHoverSelect(std::function<void()> a_fn)
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
    if (UIPointInBox(m_position, m_size, vec2((float)a_pMsg->x, (float)a_pMsg->y)) && m_clbk_Select != nullptr)
    {
      m_clbk_Select();
      a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
    }
  }

  void UIButton::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    bool isInside = UIPointInBox(m_position, m_size, vec2((float)a_pMsg->x, (float)a_pMsg->y));
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
    UIRenderer::Instance()->DrawBox(m_position, m_size, 0xFF00FFFF);
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
}