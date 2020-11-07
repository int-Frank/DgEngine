//@group UI

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <string>
#include <functional>

#include "core_utils.h"
#include "InputCodes.h"

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
      uint32_t ui32;
      bool b;
      int32_t i32;
      float f32;
      double f64;
      char* pChar;
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

  //typedef void (*UICallback)(UIWidget *);

  class UIWidget
  {
  public:

    UIWidget(mat3 const & Global_to_Local, int depth);

    virtual ~UIWidget();
    
    void SetTransform(mat3 const & trans);
    void SetDepth(int);

    int GetDepth() const;

    virtual void Draw() {}

    virtual bool Intersection(vec2 const &point);

    virtual UIState SetHoverOn() = 0;
    virtual UIState SetHoverOff() = 0;
    virtual UIState SetFocusOn() = 0;
    virtual UIState SetFocusOff() = 0;
    virtual UIState OnKeyDown(InputCode) = 0;
    virtual UIState OnKeyUp(InputCode) = 0;
    virtual UIState OnMouseDown() = 0;
    virtual UIState OnMouseUp() = 0;
    virtual UIState OnScroll(float val) = 0;
    virtual UIState OnText(char text[32]) = 0;
    virtual UIState OnActivate() = 0; // Enter key, most widgets won't have this
    UIState QueryState(); 

    void ClearBindings();
    void AddBinding(UIEvent, std::function<void(UIData const &)>);

  protected:

    virtual void OnNewTransform() {}

  protected:

    std::function<void(UIData const &)> m_callbacks[(size_t)UIEvent::COUNT];
    mat3 m_T_Global_to_Local;
    int m_depth;
    UIState m_state;
  };
}

#endif