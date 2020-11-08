//@group UI

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <string>
#include <functional>

#include "core_utils.h"
#include "EngineMessages.h"

/*
  - Button
  - Check box
  - Radio button
  - Slider
  - Drop down list
  - Scroll list
  - Image
    - Text (maybe derives from image)
  - Text Input
  - Text display
  - Text window (logging window)
*/

namespace Engine
{
  enum class UIEvent
  {
    HoverOn,
    HoverOff,
    GainFocus,
    LoseFocus,
    Activate,   //Button
    NewValue,   //Slider, Lists, Checkbox
    COUNT
  };

  enum class UIState
  {
    None,
    HoverOn,
    HasFocus // Eg text inputs in focus and waiting for input
  };

  class UIWidget
  {
  public:

    UIWidget(UIWidget * pParent);

    virtual ~UIWidget();

    void ClearBindings();
    void AddBinding(UIEvent, std::function<void(Message *)>);

    //---------------------------------------------------------------------------
    // Internal
    virtual void _Draw() {}

    //virtual void _SetHoverOn() = 0;
    //virtual void _SetHoverOff() = 0;
    //virtual void _SetFocusOn() = 0;
    //virtual void _SetFocusOff() = 0;

    void HandleMessage(Message *);
    virtual void HandleMessage(Message_GUI_GoBack *)         {}
    virtual void HandleMessage(Message_GUI_Up *)             {}
    virtual void HandleMessage(Message_GUI_Down *)           {}
    virtual void HandleMessage(Message_GUI_Left *)           {}
    virtual void HandleMessage(Message_GUI_Right *)          {}
    virtual void HandleMessage(Message_GUI_Select *)         {}
    virtual void HandleMessage(Message_GUI_PointerSelect *)  {}
    virtual void HandleMessage(Message_GUI_PointerMove *)    {}
    virtual void HandleMessage(Message_GUI_Text *)           {}

    UIState _QueryState() const;

  protected:

    virtual void OnNewTransform() {}

  protected:

    UIWidget * m_pParent;
    std::function<void(Message *)> m_callbacks[(size_t)UIEvent::COUNT];
    UIState m_state;
  };
}

#endif