//@group GUI

#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "Utils.h"
#include "GUI_Widget.h"
#include "GUI.h"

namespace DgE
{
  namespace GUI
  {
    class Button : public Widget
    {
      Button(std::string const & text, bool isGlyph, vec2 const & position, vec2 const & size, Style::Button const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static Button *Create(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {});
      static Button *Create(std::string const &text, vec2 const &position, vec2 const &size, Style::Button const &style, std::initializer_list<WidgetFlag> flags = {});

      static Button *CreateWithGlyph(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {});
      static Button *CreateWithGlyph(std::string const &text, vec2 const &position, vec2 const &size, Style::Button const &style, std::initializer_list<WidgetFlag> flags = {});

      ~Button();

      //void SetFont(uint32_t fontID);
      void SetText(std::string const &);
      void SetGlyph(std::string const &);

      static Style::Button const &GetDefaultStyle();
      Style::Button const &GetStyle() const;
      void SetStyle(Style::Button const &);

      void ClearBindings();

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void BindSelect(std::function<void()> a_fn);

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      void SetLocalPosition(vec2 const&) override;
      void SetSize(vec2 const&) override;
      vec2 GetLocalPosition() override;
      vec2 GetSize() override;

    private:

      void _HandleMessage(Message *) override;

    private:

      static Style::Button const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif