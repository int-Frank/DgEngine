//@group GUI

#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

#include "Utils.h"
#include "GUI_Widget.h"
#include "GUI.h"

namespace DgE
{
  namespace GUI
  {
    class Checkbox : public Widget
    {
      Checkbox(Widget * pParent, vec2 const & position, bool checked, Style::Checkbox const &style, std::initializer_list<WidgetFlag> flags);
    public:

      static Checkbox *Create(Widget *pParent, vec2 const &position, bool checked, std::initializer_list<WidgetFlag> flags = {});
      static Checkbox *Create(Widget *pParent, vec2 const &position, bool checked, Style::Checkbox const &, std::initializer_list<WidgetFlag> flags = {});

      ~Checkbox();

      static Style::Checkbox const &GetDefaultStyle();
      Style::Checkbox const &GetStyle() const;
      void SetStyle(Style::Checkbox const &);

      void SetChecked(bool);
      void BindCheckedChanged(std::function<void(bool)> a_fn);
      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void ClearBindings();

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      vec2 GetLocalDivPosition() override;
      vec2 GetDivSize() override;

    private:

      void _HandleMessage(Message *) override;

      void HandleMessage(Message_GUI_PointerDown *);
      void HandleMessage(Message_GUI_PointerMove *);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      static Style::Checkbox const s_style;

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif