//@group UI

#include "UICommon.h"
#include "UIWidget.h"

namespace Engine
{
  void InsertWidget(UIWidget* a_pWgt, Dg::DoublyLinkedList<UIWidget*>& a_out)
  {
    bool inserted = false;
    for (auto it = a_out.begin(); it != a_out.end(); it++)
    {
      if (a_pWgt->GetDepth() <= (*it)->GetDepth())
      {
        a_out.insert(it, a_pWgt);
        inserted = true;
      }
    }

    if (!inserted)
      a_out.push_back(a_pWgt);
  }
}