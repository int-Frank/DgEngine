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

  UIAABBType UIWidget::GetGlobalAABB(UIAABB & a_out) const
  {
    UIWidget * pParent = GetParent();
    if (pParent == nullptr)
    {
      a_out.position = GetLocalPosition();
      a_out.size = GetSize();
      return UIAABBType::FullScreen;
    }

    UIAABB aabb = {};
    if (pParent->GetGlobalAABB(aabb) == UIAABBType::None)
      return UIAABBType::None;

    if(!UIIntersection(aabb, {pParent->GetGlobalPosition() + GetLocalPosition(), GetSize()}, a_out))
      return UIAABBType::None;
    return UIAABBType::Window;
  }
}