//@group UI

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

    UIButton(UIWidget * pParent, std::string const & text, uint32_t fontID, vec2 const & position, vec2 const & size);
    ~UIButton();

    //void SetPosition(vec2 const &);
    //void SetSize(vec2 const &);
    //
    //void SetFont(uint32_t fontID);
    //void SetText(std::string const &);
    //void SetTextColour(Colour);
    //void SetHoverOnTextColour(Colour);
    //void SetBackgroundColour(Colour);
    //void SetHoverOnBackgroundColour(Colour);

    //void _SetHoverOn() override;
    //void _SetHoverOff() override;
    
    void HandleMessage(Message_GUI_PointerSelect *) override;
    void HandleMessage(Message_GUI_PointerMove *) override;

    void _Draw() override;

  private:

    bool IsInside(vec2 const &) const;

  private:

    std::string m_text;
    uint32_t m_fontID;
    vec2 m_position;
    vec2 m_size;
  };
}

#endif