//@group UI

#include "UIWidget.h"
#include "Message.h"
#include "MessageHandler.h"

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

  void UIWidget::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    DISPATCH_MESSAGE(Message_GUI_GoBack);
    DISPATCH_MESSAGE(Message_GUI_Up);
    DISPATCH_MESSAGE(Message_GUI_Down);
    DISPATCH_MESSAGE(Message_GUI_Left);
    DISPATCH_MESSAGE(Message_GUI_Right);
    DISPATCH_MESSAGE(Message_GUI_Select);
    DISPATCH_MESSAGE(Message_GUI_PointerSelect);
    DISPATCH_MESSAGE(Message_GUI_PointerMove);
    DISPATCH_MESSAGE(Message_GUI_Text);
  }

  void UIWidget::ClearBindings()
  {
    for (int i = 0; i < static_cast<int>(UIEvent::COUNT); i++)
      m_callbacks[i] = nullptr;
  }

  void UIWidget::AddBinding(UIEvent a_event, std::function<void(Message *)> a_fn)
  {
    m_callbacks[static_cast<int>(a_event)] = a_fn;
  }

  UIState UIWidget::_QueryState() const
  {
    return m_state;
  }
}