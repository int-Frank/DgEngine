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
  - Scroll list
  - Image
    - Text (maybe derives from image)
  - Text Input
  - Text display
  - Text window (logging window)
  - Image display
  - Interactive area, overlays on top of say an Image Display, and captues events
*/

// ContentDiv: Some widgets allow content to be drawn on itself
// Content can be text or images, and is not interactive.

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

    enum class WidgetFlag
    {
      NotResponsive,

      StretchWidth,
      StretchHeight,

      // Container
      Resizable,
      Movable,
      NoBackground
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

      void SetPosition(vec2 const &);
      void SetSize(vec2 const &);

      // Finds the viewable/interactable portion of the widget.
      // Return of 'false' implies none of the widget can be seen (probably completely clipped by parent widget)
      bool GetGlobalContentDivAABB(UIAABB &);
      bool GetGlobalAABB(UIAABB &);

      virtual vec2 GetContentDivPosition();
      virtual vec2 GetContentDivSize();

      vec2 GetGlobalPosition();
      vec2 GetLocalPosition();
      vec2 GetSize();
      virtual bool IsContainer() const;

      // Returns true if flag set, false if flag disallowed
      bool SetFlag(WidgetFlag, bool);
      bool HasFlag(WidgetFlag) const;

    protected:

      virtual void _HandleMessage(Message *) = 0;

      // All these will do is get/set the raw vec2.
      virtual void _SetLocalPosition(vec2 const &) = 0;
      virtual void _SetSize(vec2 const &) = 0;
      virtual vec2 _GetLocalPosition() = 0;
      virtual vec2 _GetSize() = 0;

    private:

      uint32_t m_allowedFlags;
      uint32_t m_flags;
    };
  }
}

#endif