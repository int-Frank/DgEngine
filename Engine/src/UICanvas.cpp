//@group UI

#include "UICanvas.h"
#include "UICommon.h"
#include "UIWidget.h"

namespace Engine
{
  UICanvas::UICanvas()
    : m_pFocus(nullptr)
  {

  }

  UICanvas::~UICanvas()
  {
    Clear();
  }

  void UICanvas::Clear()
  {
    for (UIWidget * ptr : m_widgets)
      delete ptr;

    m_widgets.clear();
    m_pFocus = nullptr;
  }

  void UICanvas::Add(UIWidget* a_pWgt)
  {
    InsertWidget(a_pWgt, m_widgets);
  }

  void UICanvas::HandleNewCursonPostion(float a_x, float a_y)
  {
    for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
    {
      if ((*it)->HandleNewCursonPostion(a_x, a_y))
      {
        m_pFocus = *it;
        return;
      }
    }
    m_pFocus = nullptr;
  }

  void UICanvas::Activate()
  {
    if (m_pFocus)
      m_pFocus->DoEventActivate();
  }
}