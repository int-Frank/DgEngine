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
  enum class UIState
  {
    None,
    HoverOn,
    HasFocus // Eg text inputs in focus and waiting for input
  };

  class UIWidget
  {
  public:

    virtual ~UIWidget();

    virtual void Draw() {}

    virtual void HandleMessage(Message *) {};

    virtual UIState QueryState() const = 0;
    virtual UIWidget * GetParent() const = 0;
    virtual void SetParent(UIWidget *) = 0;
    
    bool GetGlobalAABB(vec2 & a_position, vec2 & a_size) const;
    vec2 GetGlobalPosition() const;
    virtual vec2 GetLocalPosition() const = 0;
    virtual vec2 GetSize() const = 0;
  };
}

#endif