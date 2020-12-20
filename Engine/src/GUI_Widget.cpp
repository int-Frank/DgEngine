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

    bool Widget::GetGlobalAABB(UIAABB & a_out) const
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
      {
        a_out.position = GetLocalPosition();
        a_out.size = GetSize();
        return true;
      }

      UIAABB aabb ={};
      if (pParent->GetGlobalAABB(aabb) == false)
        return false;

      if (!Intersection(aabb, {pParent->GetGlobalPosition() + GetLocalPosition(), GetSize()}, a_out))
        return false;
      return true;
    }

    bool Widget::IsContainer() const
    {
      return false;
    }
    
    bool Widget::GetContentBorder(float & left, float & top, float & right, float & bottom)
    {
      return false;
    }
  }
}