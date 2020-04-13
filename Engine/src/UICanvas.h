//@group UI

#ifndef UICANVAS_H
#define UICANVAS_H

#include "DgDoublyLinkedList.h"

namespace Engine
{
  class UIWidget;

  class UICanvas
  {
  public:

    UICanvas();
    ~UICanvas();

    void Add(UIWidget*);

    void Clear();
    void HandleNewCursonPostion(float x, float y);
    void Activate();

  private:

    Dg::DoublyLinkedList<UIWidget*> m_widgets;
    UIWidget* m_pFocus;
  };
}

#endif