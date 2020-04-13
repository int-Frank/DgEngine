//@group UI

#include "UICanvas.h"
#include "UIWidget.h"
#include "Message.h"

namespace Engine
{
  UIWidget::UIWidget(std::string const& a_name,
    vec3 a_trans, vec3 a_scale,
    bool a_isInteractive, int a_depth)
    : m_hasFocus(false)
    , m_depth(a_depth)
    , m_pParent(nullptr)
    , m_translation(a_trans)
    , m_scale(a_scale)
    , m_name(a_name)
    , m_isInteractive(a_isInteractive)
    , m_bindings{0}
  {
    SetGlobalToLocal();
  }

  UIWidget::~UIWidget()
  {

  }

  bool UIWidget::IsInside(float a_x, float a_y)
  {
    vec3 point(a_x, a_y, 1.0f);
    point = point * m_T_Global_to_Local;

    bool result = point.x() > -1.0f;
    result = result && point.x() < 1.0f;
    result = result && point.y() > -1.0f;
    result = result && point.y() < 1.0f;

    return result;
  }

  void UIWidget::SetFocus(bool a_val)
  {
    m_hasFocus = a_val;
  }

  bool UIWidget::HandleNewCursonPostion(float a_x, float a_y)
  {
    return false;
  }

  bool UIWidget::HasFocus() const
  {
    return m_hasFocus;
  }

  void UIWidget::Translate(float a_x, float a_y)
  {
    m_translation += vec3(a_x, a_y, 0.0f);
    SetGlobalToLocal();
    NewTransform();
  }

  void UIWidget::Scale(float a_x, float a_y)
  {
    m_scale += vec3(a_x, a_y, 0.0f);
    SetGlobalToLocal();
    NewTransform();
  }

  void UIWidget::SetTranslation(float a_x, float a_y)
  {
    m_translation.Set(a_x, a_y, 0.0f);
    SetGlobalToLocal();
    NewTransform();
  }

  void UIWidget::SetScale(float a_x, float a_y)
  {
    m_scale.Set(a_x, a_y, 0.0f);
    SetGlobalToLocal();
    NewTransform();
  }

  void UIWidget::SetDepth(int a_val)
  {
    m_depth = a_val;
    if (m_pParent)
      m_pParent->DepthHasChanged(this);
  }

  vec3 UIWidget::GetTranslation() const
  {
    return m_translation;
  }

  vec3 UIWidget::GetScale() const
  {
    return m_scale;
  }

  int UIWidget::GetDepth() const
  {
    return m_depth;
  }

  mat3 UIWidget::GetGlobalToLocal() const
  {
    return m_T_Global_to_Local;
  }

  mat3 UIWidget::GetLocalToParent() const
  {
    mat3 mt, ms;
    mt.Translation(m_translation);
    ms.Scaling(m_scale);
    return ms * mt;
  }

  bool UIWidget::IsInteractive() const
  {
    return m_isInteractive;
  }

  void UIWidget::SetIsInteractive(bool a_val)
  {
    m_isInteractive = a_val;
  }

  void UIWidget::SetGlobalToLocal()
  {
    mat3 T_Local_to_Parent = GetLocalToParent();
    mat3 T_Local_to_Global;
    if (m_pParent)
    {
      mat3 T_Parent_to_Global = m_pParent->GetGlobalToLocal().GetInverse();
      T_Local_to_Global = T_Local_to_Parent * T_Parent_to_Global;
    }
    else
      T_Local_to_Global = T_Local_to_Parent;

    m_T_Global_to_Local = T_Local_to_Global.GetInverse();

    NewTransform();
  }

  void UIWidget::SetParent(UIWidget* a_pParent)
  {
    m_pParent = a_pParent;
  }
}