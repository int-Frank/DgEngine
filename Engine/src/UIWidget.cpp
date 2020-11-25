//@group UI

#include "UIWidget.h"
#include "Message.h"
#include "UI_Internal.h"

namespace Engine
{
  UIWidget::~UIWidget()
  {

  }

  vec2 UIWidget::GetGlobalPosition() const
  {
    UIWidget * pParent = GetParent();
    if (pParent == nullptr)
      return GetLocalPosition();
    return pParent->GetGlobalPosition() + GetLocalPosition();
  }

  bool UIWidget::GetGlobalAABB(vec2 & a_position, vec2 & a_size) const
  {
    UIWidget * pParent = GetParent();
    if (pParent == nullptr)
    {
      a_position = GetLocalPosition();
      a_size = GetSize();
      return true;
    }

    vec2 posParent, sizeParent;
    pParent->GetGlobalAABB(posParent, sizeParent);
    return UIIntersection(posParent, sizeParent, posParent + GetLocalPosition(), GetSize(), a_position, a_size);
  }
}