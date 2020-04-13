//@group UI

#include "UIButton.h"
#include "MessageBus.h"

namespace Engine
{
  UIButton::UIButton(std::string const& a_name, vec3 a_trans, vec3 a_scale)
    : UIWidget(a_name, a_trans, a_scale, true)
  {

  }

  UIButton::~UIButton()
  {

  }

  bool UIButton::HandleNewCursonPostion(float a_x, float a_y)
  {
    if (IsInside(a_x, a_y))
      return true;
    return false;
  }

  void UIButton::DoEventGainFocus()
  {
    LOG_WARN("HOVER ON");
  }

  void UIButton::DoEventLoseFocus()
  {
    LOG_WARN("HOVER OFF");
  }

  void UIButton::DoEventActivate()
  {
    LOG_ERROR("ACTIVATE");
  }

  void UIButton::Render()
  {

  }
}