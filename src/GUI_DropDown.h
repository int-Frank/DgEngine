//@group GUI

#ifndef GUI_DROPDOWN_H
#define GUI_DROPDOWN_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Dg
{
  namespace GUI
  {
    class Text;

    class DropDown : public Widget
    {
      DropDown(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, std::initializer_list<WidgetFlag> flags);
    public:

      static DropDown * Create(Widget * pParent, vec2 const & position, float width, std::initializer_list<WidgetFlag> flags ={});

      ~DropDown();

      // COlours: border, current item highlight, down button, down button background

      void AddItem(std::string const &);
      void RemoveItem(std::string const &);
      void RemoveItem(uint32_t index);

      void ClearBindings();

      void BindNewSelectedItem(std::function<void(std::string const &)> a_fn);

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

      Text * m_pDownArrow;
      Text * m_pText;
      Colour m_clrDefault;
      Colour m_clrHover;
      Colour m_clrTextDefault;
      Colour m_clrTextHover;
      UIAABB m_aabb;
      WidgetState m_state;
      float m_contentBoarder;
      Widget * m_pParent;

      std::function<void()> m_clbk_HoverOn;
      std::function<void()> m_clbk_HoverOff;
      std::function<void()> m_clbk_Select;
    };
  }
}

#endif