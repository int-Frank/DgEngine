//@group UI

#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    class Button : public Widget
    {
      Button(Widget * pParent, std::string const & text, uint32_t fontID, vec2 const & position, vec2 const & size);
    public:

      static Button * Create(Widget * pParent, std::string const & text, uint32_t fontID, vec2 const & position, vec2 const & size);

      ~Button();

      void SetPosition(vec2 const &) override;
      void SetSize(vec2 const &) override;
      void SetBackgroundColour(Colour);
      void SetHoverOnBackgroundColour(Colour);

      //void SetFont(uint32_t fontID);
      //void SetText(std::string const &);
      //void SetTextColour(Colour);
      //void SetHoverOnTextColour(Colour);

      void ClearBindings();

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);
      void BindSelect(std::function<void()> a_fn);

      void HandleMessage(Message *) override;

      void Draw() override;
      vec2 GetLocalPosition() const override;
      vec2 GetSize() const override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

    private:

      void HandleMessage(Message_GUI_PointerDown *);
      void HandleMessage(Message_GUI_PointerMove *);

    private:

      std::string m_text;
      uint32_t m_fontID;
      Colour m_clrDefault;
      Colour m_clrHover;
      UIAABB m_aabb;
      WidgetState m_state;
      Widget * m_pParent;

      std::function<void()> m_clbk_HoverOn;
      std::function<void()> m_clbk_HoverOff;
      std::function<void()> m_clbk_Select;
    };
  }
}

#endif