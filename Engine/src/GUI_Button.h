//@group GUI

#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    class Text;

    enum class ButtonState
    {
      Normal,
      Hover,

      COUNT
    };

    enum class ButtonElement
    {
      Face,
      Outline,
      Text,

      COUNT
    };

    class Button : public Widget
    {
      Button(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, std::initializer_list<WidgetFlag> flags);
    public:

      static Button * Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, std::initializer_list<WidgetFlag> flags = {});

      ~Button();

      //void SetFont(uint32_t fontID);
      void SetText(std::string const &);
      void SetContentMargin(float);
      void SetColour(ButtonState, ButtonElement, Colour);

      void ClearBindings();

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void BindSelect(std::function<void()> a_fn);

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

      Text * m_pText;
      UIAABB m_aabb;
      WidgetState m_state;
      Widget * m_pParent;

      Colour m_clr[(int)ButtonState::COUNT][(int)ButtonElement::COUNT];
      float m_contentMargin;
      float m_outlineWidth;

      std::function<void()> m_clbk_HoverOn;
      std::function<void()> m_clbk_HoverOff;
      std::function<void()> m_clbk_Select;
    };
  }
}

#endif