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
    class Container : public Widget
    {
      Container(Widget * pParent, vec2 const position, vec2 const & size, Style::Container const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static vec2 const s_minSize;

      static Container *Create(Widget *pParent, vec2 const position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable});
      static Container *Create(Widget *pParent, vec2 const position, vec2 const &size, Style::Container const &, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable});
      
      ~Container();

      static Style::Container const &GetDefaultStyle();
      Style::Container const &GetStyle() const;
      void SetStyle(Style::Container const &);

      void Clear();
      void Add(Widget *); // TODO Add check if widget already exists.
      void Remove(Widget *);

      void Draw() override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;
      vec2 GetContentDivPosition() override;
      vec2 GetContentDivSize() override;

      bool IsContainer() const override;

    private:

      void _HandleMessage(Message *) override;

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      static Style::Container const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif