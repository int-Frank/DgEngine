//@group UI

#include "GUI_Widget.h"
#include "Message.h"
#include "GUI_Internal.h"

namespace Engine
{
  namespace GUI
  {
    Widget::~Widget()
    {

    }

    vec2 Widget::GetGlobalPosition() const
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
        return GetLocalPosition();
      return pParent->GetGlobalPosition() + GetLocalPosition();
    }

    AABBType Widget::GetGlobalAABB(UIAABB & a_out) const
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
      {
        a_out.position = GetLocalPosition();
        a_out.size = GetSize();
        return AABBType::FullScreen;
      }

      UIAABB aabb ={};
      if (pParent->GetGlobalAABB(aabb) == AABBType::None)
        return AABBType::None;

      if (!Intersection(aabb, {pParent->GetGlobalPosition() + GetLocalPosition(), GetSize()}, a_out))
        return AABBType::None;
      return AABBType::Window;
    }

    bool Widget::IsWindow() const
    {
      return false;
    }
  }
}