//@group UI

#include "UIFrame.h"
#include "UICommon.h"
#include "UIWidget.h"
#include "UI_Internal.h"

#define ADD_MSG_HANDLER(TYPE) void UIFrame::HandleMessage(TYPE * a_pMsg)\
  {\
    for (UIWidget * ptr : m_widgets)\
    {\
      ptr->HandleMessage(a_pMsg);\
      if (ptr->QueryState() == UIState::HasFocus)\
      {\
        m_pFocus = ptr;\
        break;\
      }\
      if (a_pMsg->QueryFlag(Message::Flag::Handled))\
        break;\
    }\
  }



namespace Engine
{
  UIFrame::UIFrame()
    : m_widgets()
    , m_pFocus(nullptr)
  {

  }

  UIFrame::~UIFrame()
  {
    Clear();
  }

  void UIFrame::Clear()
  {
    for (UIWidget * ptr : m_widgets)
      delete ptr;

    m_widgets.clear();
    m_pFocus = nullptr;
  }

  void UIFrame::Add(UIWidget* a_pWgt)
  {
    m_widgets.push_back(a_pWgt);
  }

  void UIFrame::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return;

    if (m_pFocus != nullptr)
    {
      m_pFocus->HandleMessage(a_pMsg);
      if (m_pFocus->QueryState() == UIState::None)
        m_pFocus = nullptr;
      return;
    }

    DISPATCH_MESSAGE(Message_GUI_Up);
    DISPATCH_MESSAGE(Message_GUI_Down);
    DISPATCH_MESSAGE(Message_GUI_Left);
    DISPATCH_MESSAGE(Message_GUI_Right);
    DISPATCH_MESSAGE(Message_GUI_PointerUp);
    DISPATCH_MESSAGE(Message_GUI_PointerDown);
    DISPATCH_MESSAGE(Message_GUI_PointerMove);
    DISPATCH_MESSAGE(Message_GUI_Text);
  }

  ADD_MSG_HANDLER(Message_GUI_Up)
  ADD_MSG_HANDLER(Message_GUI_Down)
  ADD_MSG_HANDLER(Message_GUI_Left)
  ADD_MSG_HANDLER(Message_GUI_Right)
  ADD_MSG_HANDLER(Message_GUI_PointerUp)
  ADD_MSG_HANDLER(Message_GUI_PointerDown)
  ADD_MSG_HANDLER(Message_GUI_PointerMove)
  ADD_MSG_HANDLER(Message_GUI_Text)

  void UIFrame::Draw()
  {
    for (UIWidget * pWidget : m_widgets)
      pWidget->Draw();

    //Unbind UI texture
  }
}