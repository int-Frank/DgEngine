//@group UI

#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "core_utils.h"
#include "UIWidget.h"

namespace Engine
{
  class UIButton : public UIWidget
  {
  public:

    UIButton(std::string const& name, vec3 trans, vec3 scale);
    ~UIButton();

    bool HandleNewCursonPostion(float x, float y) override;
    void DoEventGainFocus() override;
    void DoEventLoseFocus() override;
    void DoEventActivate() override;

    void Render() override;

    void SetText(std::string const& text);
    void SetTextSize(int);
    void SetFont(uint32_t id);
    void SetBackgroundColor(RGBA normal, RGBA hover);
    void SetTextColor(RGBA normal, RGBA hover);
  };
}

#endif