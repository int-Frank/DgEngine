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
      Window(Widget * pParent, vec2 const position, vec2 const & size, Style::Window const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static vec2 const s_minSize;

      static Window *Create(Widget *pParent, vec2 const position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable});
      static Window *Create(Widget *pParent, vec2 const position, vec2 const &size, Style::Window const &, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable});
      
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
      vec2 GetLocalDivPosition() override;
      vec2 GetDivSize() override;

      bool IsContainer() const override;

    private:

      void _HandleMessage(Message *) override;

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      static Style::Window const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif