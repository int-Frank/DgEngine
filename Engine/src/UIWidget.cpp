//@group UI
//
//#include "UICanvas.h"
//#include "UIWidget.h"
//#include "Message.h"
//
//namespace Engine
//{
//  UIWidget::UIWidget(mat3 const & a_Global_to_Local, int a_depth)
//    : m_bindings{0}
//    , m_T_Global_to_Local(a_Global_to_Local)
//    , m_depth(a_depth)
//  {
//
//  }
//
//  UIWidget::~UIWidget()
//  {
//
//  }
//
//  bool UIWidget::IsInside(vec2 const &a_point)
//  {
//    vec3 point(a_point.x(), a_point.y(), 1.0f);
//    point = point * m_T_Global_to_Local;
//
//    bool result = point.x() > -1.0f;
//    result = result && point.x() < 1.0f;
//    result = result && point.y() > -1.0f;
//    result = result && point.y() < 1.0f;
//
//    return result;
//  }
//
//  void UIWidget::SetTransform(mat3 const & a_trans)
//  {
//    m_T_Global_to_Local = a_trans;
//  }
//
//  void UIWidget::SetDepth(int a_val)
//  {
//    m_depth = a_val;
//  }
//
//  int UIWidget::GetDepth() const
//  {
//    return m_depth;
//  }
//
//  void UIWidget::SetCallback(UICallback a_ui, fnUICallback a_fn)
//  {
//    m_bindings[static_cast<size_t>(a_ui)] = a_fn;
//  }
//}