//@group UI

#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "core_utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    class WindowState;

    class Window : public Widget
    {
      Window(Widget * pParent, vec2 const position, vec2 const & size, uint32_t flags);
    public:

      static vec2 const s_minSize;

      enum Flag : uint32_t
      {
        Resizable     = (1u << 0),
        Movable       = (1u << 1),
        NoBackground  = (1u << 2),

        COUNT
      };

      static Window * Create(Widget * pParent, vec2 const position, vec2 const & size, uint32_t flags = Resizable | Movable);
      ~Window();

      void SetPosition(vec2 const &) override;
      void SetSize(vec2 const &) override;

      void SetBackgroundColour(Colour);
      //void SetHoverOnBackgroundColour(Colour);

      void Clear();
      void Add(Widget *);
      void Remove(Widget *);

      void HandleMessage(Message *);

      void Draw() override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;
      vec2 GetLocalPosition() const override;
      vec2 GetSize() const override;

      bool IsWindow() const override;

    private:

      void UpdateState(WindowState *);

    private:

      WindowState * m_pState;
    };
  }
}

#endif