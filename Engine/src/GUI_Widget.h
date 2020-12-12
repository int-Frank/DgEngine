//@group UI

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <string>
#include <functional>

#include "Utils.h"
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
  - Image display
  - Interactive area, overlays on top of say an Image Display, and captues events
*/

namespace Engine
{
  namespace GUI
  {
    enum class WidgetState
    {
      None,
      HoverOn,
      HasFocus // Eg text inputs in focus and waiting for input
    };

    enum class AABBType
    {
      FullScreen,
      Window,
      None
    };

    class Widget
    {
    public:

      virtual void SetPosition(vec2 const &) = 0;
      virtual void SetSize(vec2 const &) = 0;

      // Internal use...
      virtual ~Widget();

      virtual void HandleMessage(Message *) { };
      virtual void Draw() { }
      virtual WidgetState QueryState() const = 0;
      virtual Widget * GetParent() const = 0;
      virtual void SetParent(Widget *) = 0;

      AABBType GetGlobalAABB(UIAABB &) const;
      vec2 GetGlobalPosition() const;
      virtual vec2 GetLocalPosition() const = 0;
      virtual vec2 GetSize() const = 0;
      virtual bool IsWindow() const;
    };
  }
}

#endif