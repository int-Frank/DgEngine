//@group GUI

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "Utils.h"
#include "GUI_Widget.h"
#include "GUI.h"

namespace DgE
{
  namespace GUI
  {
    // Should be called Window
    class Window : public Widget
    {
      Window(vec2 const position, vec2 const & size, Style::Window const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static Window *Create(vec2 const position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable, WidgetFlag::VerticalScroll, WidgetFlag::HorizontalScroll});
      static Window *Create(vec2 const position, vec2 const &size, Style::Window const &, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable, WidgetFlag::VerticalScroll, WidgetFlag::HorizontalScroll});
      
      ~Window();

      static Style::Window const &GetDefaultStyle();
      Style::Window const &GetStyle() const;
      void SetStyle(Style::Window const &);

      void Clear();
      void Add(Widget *); // TODO Add check if widget already exists.
      void Remove(Widget *);

      void Draw() override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      bool IsContainer() const override;

      void SetLocalPosition(vec2 const&) override;
      void SetSize(vec2 const&) override;
      vec2 GetLocalPosition() override;
      vec2 GetSize() override;

      static Style::Window const s_style;

    private:

      void _HandleMessage(Message *) override;

    private:

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif