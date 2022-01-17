//@group GUI

#include "GUI_Widget.h"
#include "Message.h"
#include "GUI_Internal.h"

namespace DgE
{
  namespace GUI
  {
    Widget::Widget(std::initializer_list<WidgetFlag> a_allowedFlags,
                   std::initializer_list<WidgetFlag> a_flags)
      : m_allowedFlags(0)
      , m_flags(0)
    {
      for (WidgetFlag flag : a_allowedFlags)
        m_allowedFlags |= (1ul << (uint32_t)flag);

      for (WidgetFlag flag : a_flags)
        SetFlag(flag, true);
    }

    Widget::~Widget()
    {

    }

    void Widget::UpdateSize()
    {
      Widget* pParent = GetParent();
      if (pParent)
      {
        vec2 parentSize = pParent->GetSize();
        vec2 thisSize = GetSize();
        bool dirty = false;
        if (HasFlag(WidgetFlag::StretchWidth))
        {
          dirty = true;
          thisSize.x() = parentSize.x();
        }
        if (HasFlag(WidgetFlag::StretchHeight))
        {
          dirty = true;
          thisSize.y() = parentSize.y();
        }
        if (dirty)
          SetSize(thisSize);
      }
    }

    vec2 Widget::GetGlobalPosition()
    {
      vec2 result = GetLocalPosition();

      Widget * pParent = GetParent();
      if (pParent != nullptr)
        result += pParent->GetGlobalPosition();
      return result;
    }

    bool Widget::GetGlobalViewableArea(UIAABB & a_out)
    {
      Widget * pParent = GetParent();
      if (pParent == nullptr)
      {
        a_out.position = GetLocalPosition();
        a_out.size = GetSize();
        return true;
      }

      UIAABB aabb ={};
      if (pParent->GetGlobalViewableArea(aabb) == false)
        return false;

      if (!Intersection(aabb, {GetGlobalPosition(), GetSize()}, a_out))
        return false;
      return true;
    }

    bool Widget::IsContainer() const
    {
      return false;
    }

    bool Widget::HasFlag(WidgetFlag a_flag) const
    {
      return (m_flags & (1ul << (uint32_t)a_flag)) != 0;
    }

    bool Widget::SetFlag(WidgetFlag a_flag, bool a_set)
    {
      if ((m_allowedFlags & (1ul << (uint32_t)a_flag)) != 0)
      {
        if (a_set)
          m_flags |= (1ul << (uint32_t)a_flag);
        else
          m_flags &= ~(1ul << (uint32_t)a_flag);

        return true;
      }
      return false;
    }

    void Widget::HandleMessage(Message * a_pMsg)
    {
      if (HasFlag(WidgetFlag::NotResponsive))
        return;

      _HandleMessage(a_pMsg);
    }
  }
}