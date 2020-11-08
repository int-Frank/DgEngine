//@group UI

#include "UIButton.h"
#include "MessageHandler.h"

namespace Engine
{
  UIButton::UIButton(UIWidget * a_pParent, std::string const & a_text, uint32_t a_fontID, vec2 const & a_position, vec2 const & a_size)
    : UIWidget(a_pParent)
    , m_text(a_text)
    , m_fontID(a_fontID)
    , m_position(a_position)
    , m_size(a_size)
  {

  }

  UIButton::~UIButton()
  {

  }

  void UIButton::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    DISPATCH_MESSAGE(Message_GUI_PointerMove);
    DISPATCH_MESSAGE(Message_GUI_PointerSelect);
  }

  void UIButton::HandleMessage(Message_GUI_PointerSelect * a_pMsg)
  {
    if (IsInside(vec2((float)a_pMsg->x, (float)a_pMsg->y)) 
      && m_callbacks[static_cast<int>(UIEvent::Activate)] != nullptr)
    {
      UICallbackData data;
      data.pointerSelect.x = (float)a_pMsg->x;
      data.pointerSelect.y = (float)a_pMsg->y;
      m_callbacks[static_cast<int>(UIEvent::Activate)](&data);
    }
  }

  void UIButton::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    bool isInside = IsInside(vec2((float)a_pMsg->x, (float)a_pMsg->y));
    if (isInside && m_state == UIState::None)
    {
      m_state = UIState::HoverOn;
      if (m_callbacks[static_cast<int>(UIEvent::HoverOn)] != nullptr)
        m_callbacks[static_cast<int>(UIEvent::HoverOn)](nullptr);
    }
    if (!isInside && m_state == UIState::HoverOn)
    {
      m_state = UIState::None;
      if (m_callbacks[static_cast<int>(UIEvent::HoverOff)] != nullptr)
        m_callbacks[static_cast<int>(UIEvent::HoverOff)](nullptr);
    }
  }

  bool UIButton::IsInside(vec2 const & a_point) const
  {
    if (a_point.x() < m_position.x())
      return false;

    if (a_point.y() < m_position.y())
      return false;

    if (a_point.x() > m_position.x() + m_size.x())
      return false;

    if (a_point.y() > m_position.y() + m_size.y())
      return false;

    return true;
  }

  void UIButton::_Draw()
  {

  }
}