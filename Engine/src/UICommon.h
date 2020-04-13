//@group UI

#ifndef UICOMMON_H
#define UICOMMON_H

#include "DgDoublyLinkedList.h"

namespace Engine
{
  class UIWidget;
  extern void InsertWidget(UIWidget*, Dg::DoublyLinkedList<UIWidget*>& out);
}

#endif