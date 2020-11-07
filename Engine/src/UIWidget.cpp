//@group UI

#include "UIForm.h"
#include "UIWidget.h"
#include "Message.h"

namespace Engine
{
  UIWidget::UIWidget(mat3 const & a_Global_to_Local, int a_depth)
    : m_callbacks{}
    , m_T_Global_to_Local(a_Global_to_Local)
    , m_depth(a_depth)
    , m_state(UIState::None)
  {

  }

  UIWidget::~UIWidget()
  {

  }

  bool UIWidget::Intersection(vec2 const &a_point)
  {
    vec3 point(a_point.x(), a_point.y(), 1.0f);
    point = point * m_T_Global_to_Local;

    bool result = point.x() > -1.0f;
    result = result && point.x() < 1.0f;
    result = result && point.y() > -1.0f;
    result = result && point.y() < 1.0f;

    return result;
  }

  void UIWidget::SetTransform(mat3 const & a_trans)
  {
    m_T_Global_to_Local = a_trans;
  }

  void UIWidget::SetDepth(int a_val)
  {
    m_depth = a_val;
  }

  int UIWidget::GetDepth() const
  {
    return m_depth;
  }
}