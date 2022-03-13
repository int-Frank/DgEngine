//@group GUI

#ifndef GUI_RADIOBUTTON_H
#define GUI_RADIOBUTTON_H

#include "Utils.h"
#include "GUI_Widget.h"
#include "GUI.h"

namespace DgE
{
  namespace GUI
  {
    class RadioButton : public Widget
    {
      RadioButton(vec2 const &position, float size, Style::RadioButton const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static RadioButton *Create(RadioButton *pSibling, int id, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags = {});
      static RadioButton *Create(RadioButton *pSibling, int id, vec2 const &position, vec2 const &size, Style::Button const &style, std::initializer_list<WidgetFlag> flags = {});

      ~RadioButton();

      static Style::Button const &GetDefaultStyle();
      Style::Button const &GetStyle() const;
      void SetStyle(Style::Button const &);

      void ClearBindings();

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void BindNewValue(std::function<void(int)> a_fn);

      void Draw() override;
      WidgetState QueryState() const override;
      Widget *GetParent() const override;
      void SetParent(Widget *) override;

      void SetLocalPosition(vec2 const &) override;
      void SetSize(vec2 const &) override;
      vec2 GetLocalPosition() override;
      vec2 GetSize() override;

    private:

      void _HandleMessage(Message *) override;

    private:

      static Style::RadioButton const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif