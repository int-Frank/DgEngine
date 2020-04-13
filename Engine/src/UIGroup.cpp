//@group UI

#include "UIGroup.h"
#include "UIWidget.h"
#include "UICommon.h"

namespace Engine
{
  UIGroup::UIGroup(std::string const& a_name, vec3 a_trans, vec3 a_scale)
    : UIWidget(a_name, a_trans, a_scale, false)
    , m_pFocus(nullptr)
  {

  }

  UIGroup::~UIGroup()
  {
    Clear();
  }

  bool UIGroup::IsInside(float a_x, float a_y)
  {
    vec3 point(a_x, a_y, 1.0f);
    point = point * m_T_Global_to_Interactive;

    bool result = point.x() > 0.0f;
    result = result && point.x() < 1.0f;
    result = result && point.y() > 0.0f;
    result = result && point.y() < 1.0f;

    return result;
  }

  bool UIGroup::HandleNewCursonPostion(float a_x, float a_y)
  {
    UIWidget* focus = nullptr;

    if (!IsInside(a_x, a_y))
      goto epilogue;

    for (UIWidget* wgt : m_children)
    {
      if (wgt->HandleNewCursonPostion(a_x, a_y))
      {
        focus = wgt;
        break;
      }
    }

  epilogue:

    if (focus != m_pFocus)
    {
      if (m_pFocus)
        m_pFocus->DoEventLoseFocus();

      m_pFocus = focus;

      if (m_pFocus)
        m_pFocus->DoEventGainFocus();
    }

    return focus != nullptr;
  }

  void UIGroup::DoEventActivate()
  {
    if (m_pFocus)
      m_pFocus->DoEventActivate();
  }

  void UIGroup::Render()
  {
    for (UIWidget* child : m_children)
      child->Render();
  }

  void UIGroup::Clear()
  {
    for (UIWidget* child : m_children)
      delete child;
    m_children.clear();
    m_pFocus = nullptr;
    SetInteractiveSpace();
  }

  void UIGroup::Add(UIWidget* a_pWgt)
  {
    InsertWidget(a_pWgt, m_children);
    a_pWgt->SetParent(this);
    a_pWgt->SetGlobalToLocal();
    a_pWgt->NewTransform();
    SetInteractiveSpace();
  }

  bool UIGroup::Remove(UIWidget* a_pWgt)
  {
    for (auto it = m_children.begin(); it != m_children.end(); it++)
    {
      if ((*it) == a_pWgt)
      {
        m_children.erase(it);
        SetInteractiveSpace();
        return true;
      }
    }
    return false;
  }

  void UIGroup::DepthHasChanged(UIWidget* a_pWgt)
  {
    if (Remove(a_pWgt))
      Add(a_pWgt);
  }

  void UIGroup::NewTransform()
  {
    for (UIWidget* child : m_children)
      child->SetGlobalToLocal();
  }

  void UIGroup::SetInteractiveSpace()
  {
    m_isInteractive = false;
    Dg::DoublyLinkedList<UIWidget*>::iterator it = m_children.begin();

    vec3 minPoint;
    vec3 maxPoint;

    for (; it != m_children.end(); it++)
    {
      if ((*it)->IsInteractive())
      {
        m_isInteractive = true;
        minPoint = (*it)->GetTranslation();
        maxPoint = minPoint + (*it)->GetScale();
        it++;
        break;
      }
    }

    for (; it != m_children.end(); it++)
    {
      if ((*it)->IsInteractive())
      {
        vec3 newMinPoint = (*it)->GetTranslation();
        vec3 newMaxPoint = minPoint + (*it)->GetScale();
        for (int i = 0; i < 2; i++)
        {
          if (newMinPoint[i] < minPoint[i])
            minPoint[i] = newMinPoint[i];
          if (newMaxPoint[i] > maxPoint[i])
            maxPoint[i] = newMaxPoint[i];
        }
      }
    }

    if (m_isInteractive)
    {
      mat3 mt, ms;
      vec3 scale = maxPoint - minPoint;
      scale[2] = 1.0f;
      mt.Translation(minPoint);
      ms.Scaling(scale);
      mat3 T_Interactive_to_Local = ms * mt;
      mat3 T_Local_to_Interactive = T_Interactive_to_Local.GetInverse();
      m_T_Global_to_Interactive = m_T_Global_to_Local * T_Local_to_Interactive;
    }
  }
}