//@group GUI

#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

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
  - Image
    - Text (maybe derives from image)
  - Text Input
  - Text display
  - Text window (logging window)
  - Image display
*/

// ContentDiv: Some widgets allow content to be drawn on itself
// Content can be text or images, and is not interactive.

namespace DgE
{
  namespace GUI
  {
    enum class WidgetState
    {
      None,
      HoverOn,
      HasFocus // Eg text inputs in focus and waiting for input
    };

    enum class WidgetFlag
    {
      NotResponsive,

      StretchWidth,
      StretchHeight,

      // Window
      Resizable,
      Movable,
      VerticalScroll,
      HorizontalScroll
    };

    class Widget
    {
    public:

      Widget(std::initializer_list<WidgetFlag> allowedFlags, std::initializer_list<WidgetFlag> flags);
      virtual ~Widget();

      void HandleMessage(Message *);
      virtual void Draw() { }
      virtual WidgetState QueryState() const = 0;
      virtual Widget * GetParent() const = 0;
      virtual void SetParent(Widget *) = 0;

      virtual vec2 GetLocalPosition() = 0;
      virtual void SetLocalPosition(vec2 const &) = 0;
      virtual vec2 GetSize() = 0;
      virtual void SetSize(vec2 const &) = 0;

      void UpdateSize();

      // Finds the viewable/interactable portion of the widget.
      // Return of 'false' implies none of the widget can be seen (probably completely clipped by parent widget)
      bool GetGlobalViewableArea(UIAABB &);
      vec2 GetGlobalPosition();

      virtual bool IsContainer() const;

      // Returns true if flag set, false if flag disallowed
      bool SetFlag(WidgetFlag, bool);
      bool HasFlag(WidgetFlag) const;

    protected:

      virtual void _HandleMessage(Message *) = 0;

    private:

      uint32_t m_allowedFlags;
      uint32_t m_flags;
    };
  }
}

#endif