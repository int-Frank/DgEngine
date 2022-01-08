//@group GUI

#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "Utils.h"
#include "GUI_Widget.h"
#include "GUI.h"

namespace DgE
{
  namespace GUI
  {
    class Text : public Widget
    {
      Text(std::string const & text, vec2 const & position, vec2 const & size, Style::Text const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static Text *Create(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {});
      static Text *Create(std::string const &text, vec2 const &position, vec2 const &size, Style::Text const &style, std::initializer_list<WidgetFlag> flags = {});

      ~Text();

      void SetText(std::string const &);

      static Style::Text const &GetDefaultStyle();
      Style::Text const &GetStyle() const;
      void SetStyle(Style::Text const&);

      //void SetFont(FontID fontID, uint32_t size);
      void SetGlyphSize(uint32_t size);
      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

    private:

      void _HandleMessage(Message *) override;

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      static Style::Text const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif