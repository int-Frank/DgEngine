//@group GUI

#include "GUI_Widget.h"
#include "Message.h"
#include "GUI_Internal.h"

namespace Engine
{
  namespace GUI
  {
    Widget::Widget(std::initializer_list<WidgetFlag> a_flags)
      : m_flags(0)
    {
      for (WidgetFlag flag : a_flags)
        m_flags |= (1ul << (uint32_t)flag);
    }

    Widget::~Widget()
    {

    }

    vec2 Widget::GetGlobalPosition()
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
        return GetLocalPosition();
      return pParent->GetGlobalPosition() + GetLocalPosition();
    }

    bool Widget::GetGlobalAABB(UIAABB & a_out)
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

    bool Widget::GetGlobalContentDivAABB(UIAABB & a_out)
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
      {
        a_out.position = GetLocalPosition() + GetContentDivPosition();
        a_out.size = GetContentDivSize();
        return true;
      }

      UIAABB aabb ={};
      if (pParent->GetGlobalAABB(aabb) == false)
        return false;

      if (!Intersection(aabb, {pParent->GetGlobalPosition() + GetLocalPosition(), GetContentDivSize()}, a_out))
        return false;
      return true;
    }

    vec2 Widget::GetContentDivPosition()
    {
      return vec2::ZeroVector();
    }

    vec2 Widget::GetContentDivSize()
    {
      return vec2::ZeroVector();
    }

    bool Widget::IsContainer() const
    {
      return false;
    }

    bool Widget::HasFlag(WidgetFlag a_flag) const
    {
      return (m_flags & (1ul << (uint32_t)a_flag)) != 0;
    }

    vec2 Widget::GetLocalPosition()
    {
      Widget *pParent = GetParent();
      if (pParent != nullptr)
      {
        if (HasFlag(WidgetFlag::StretchWidth) || HasFlag(WidgetFlag::StretchHeight))
        {
          vec2 contentPos = pParent->GetContentDivPosition();
          vec2 position = _GetLocalPosition();

          if (HasFlag(WidgetFlag::StretchWidth))
            position[0] = contentPos[0];
          if (HasFlag(WidgetFlag::StretchHeight))
            position[1] = contentPos[1];

          _SetLocalPosition(position);
        }
      }

      return _GetLocalPosition();
    }

    vec2 Widget::GetSize()
    {
      Widget * pParent = GetParent();
      if (pParent != nullptr)
      {
        if (HasFlag(WidgetFlag::StretchWidth) || HasFlag(WidgetFlag::StretchHeight))
        {
          vec2 contentSize = pParent->GetContentDivSize();
          vec2 size = _GetSize();

          if (HasFlag(WidgetFlag::StretchWidth))
            size[0] = contentSize[0];
          if (HasFlag(WidgetFlag::StretchHeight))
            size[1] = contentSize[1];

          _SetSize(size);
        }
      }

      return _GetSize();
    }

    void Widget::SetPosition(vec2 const & a_pos)
    {
      if (HasFlag(WidgetFlag::StretchHeight) || HasFlag(WidgetFlag::StretchWidth))
        return;

      _SetLocalPosition(a_pos);
    }

    void Widget::SetSize(vec2 const & a_size)
    {
      if (HasFlag(WidgetFlag::StretchHeight) || HasFlag(WidgetFlag::StretchWidth))
        return;

      _SetSize(a_size);
    }

    void Widget::HandleMessage(Message * a_pMsg)
    {
      if (HasFlag(WidgetFlag::NotResponsive))
        return;

      _HandleMessage(a_pMsg);
    }
  }
}