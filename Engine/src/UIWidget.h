//@group UI

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <string>
#include <functional>

#include "core_utils.h"
#include "EngineMessages.h"
#include "UICommon.h"

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

  enum class UIAABBType
  {
    FullScreen,
    Window,
    None
  };

  class UIWidget
  {
  public:

    virtual ~UIWidget();

    virtual void Draw() {}

    virtual void HandleMessage(Message *) {};

    virtual void SetPosition(vec2 const &) = 0;
    virtual void SetSize(vec2 const &) = 0;

    virtual UIState QueryState() const = 0;
    virtual UIWidget * GetParent() const = 0;
    virtual void SetParent(UIWidget *) = 0;
    
    UIAABBType GetGlobalAABB(UIAABB &) const;
    vec2 GetGlobalPosition() const;
    virtual vec2 GetLocalPosition() const = 0;
    virtual vec2 GetSize() const = 0;
  };
}

#endif