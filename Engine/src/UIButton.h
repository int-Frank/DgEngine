@group UI

#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "UICommon.h"
#include "core_utils.h"
#include "UIWidget.h"

namespace Engine
{
  class UIButton : public UIWidget
  {
  public:

    enum State
    {
      Default,
      Hover,

      Count
    };

    UIButton(UIWidget * pParent, vec2i const & position, vec2i const & size);
    ~UIButton();

    void SetPosition(vec2i const &);
    void SetSize(vec2i const &);

    void SetFont(uint32_t fontID);
    void SetText(std::string const &);
    void SetTextColour(State, Colour);
    void SetBackgroundColour(State, Colour)

    UIState HoverOn() override;
    UIState HoverOff() override;
    UIState DoMouseDown() override;

    void Draw() override;


  private:

  };
}

#endif