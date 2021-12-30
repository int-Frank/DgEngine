//@group GUI

#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace DgE
{
  namespace GUI
  {
    enum class CheckboxState
    {
      Normal,
      Hover,

      COUNT
    };

    enum class CheckboxElement
    {
      Outline,
      Tick,

      COUNT
    };

    class Text;

    class CheckBox : public Widget
    {
      CheckBox(Widget * pParent, vec2 const & position, bool checked, std::initializer_list<WidgetFlag> flags);
    public:

      static CheckBox * Create(Widget * pParent, vec2 const & position, bool checked, std::initializer_list<WidgetFlag> flags ={});

      ~CheckBox();

      void SetChecked(bool);
      void SetColour(CheckboxState, CheckboxElement, Colour);
      void BindCheckedChanged(std::function<void(bool)> a_fn);
      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void ClearBindings();

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      vec2 GetContentDivPosition() override;
      vec2 GetContentDivSize() override;

    private:

      void _HandleMessage(Message *) override;

      void HandleMessage(Message_GUI_PointerDown *);
      void HandleMessage(Message_GUI_PointerMove *);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      bool m_isChecked;
      Text * m_pTextTick;
      Colour m_clr[(int)CheckboxState::COUNT][(int)CheckboxElement::COUNT];
      UIAABB m_aabb;
      WidgetState m_state;
      Widget * m_pParent;

      std::function<void(bool)> m_clbk_CheckChanged;
      std::function<void()> m_clbk_HoverOn;
      std::function<void()> m_clbk_HoverOff;
    };
  }
}

#endif