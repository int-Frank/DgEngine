//@group UI

#include "UIWidget.h"
#include "Message.h"

namespace Engine
{
  UIWidget::UIWidget(UIWidget * a_pParent)
    : m_pParent(a_pParent)
    , m_callbacks{}
    , m_state(UIState::None)
  {

  }

  UIWidget::~UIWidget()
  {

  }

  void UIWidget::ClearBindings()
  {
    for (int i = 0; i < static_cast<int>(UIEvent::COUNT); i++)
      m_callbacks[i] = nullptr;
  }

  void UIWidget::AddBinding(UIEvent a_event, std::function<void(UICallbackData const *)> a_fn)
  {
    m_callbacks[static_cast<int>(a_event)] = a_fn;
  }

  UIState UIWidget::_QueryState() const
  {
    return m_state;
  }
}