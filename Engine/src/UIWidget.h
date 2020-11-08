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
  struct UIData
  {
    union
    {
      struct PointerSelect
      {
        float x, y;
      } pointerSelect;
    };
  };

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
    void AddBinding(UIEvent, std::function<void(UIData const *)>);

    //---------------------------------------------------------------------------
    // Internal
    virtual void _Draw() {}

    //virtual void _SetHoverOn() = 0;
    //virtual void _SetHoverOff() = 0;
    //virtual void _SetFocusOn() = 0;
    //virtual void _SetFocusOff() = 0;

    virtual void HandleMessage(Message *) {};

    UIState _QueryState() const;

  protected:

    virtual void OnNewTransform() {}

  protected:

    UIWidget * m_pParent;
    std::function<void(UIData const *)> m_callbacks[(size_t)UIEvent::COUNT];
    UIState m_state;
  };
}

#endif