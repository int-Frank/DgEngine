//@group UI

#ifndef UIGROUPS_H
#define UIGROUPS_H

#include "core_utils.h"
#include "UIWidget.h"
#include "DgDoublyLinkedList.h"

namespace Engine
{
  class UIGroup : public UIWidget
  {
  public:

    UIGroup(UIWidget * pParent, vec2 const position, vec2 const & size);
    ~UIGroup();

    //void SetPosition(vec2 const &);
    //void SetSize(vec2 const &);
    //
    //void SetFont(uint32_t fontID);
    //void SetText(std::string const &);
    //void SetTextColour(Colour);
    //void SetHoverOnTextColour(Colour);
    //void SetBackgroundColour(Colour);
    //void SetHoverOnBackgroundColour(Colour);

    //void _SetHoverOn() override;
    //void _SetHoverOff() override;

    void Clear();
    void Add(UIWidget *);
    bool Remove(UIWidget *);

    void HandleMessage(Message_GUI_PointerSelect *);
    void HandleMessage(Message_GUI_PointerMove *);
    void HandleMessage(Message_GUI_Select *);

    void _Draw() override;

  private:

    bool IsInside(vec2 const &) const;

  private:

    vec2 m_position;
    vec2 m_size;
    Dg::DoublyLinkedList<UIWidget *> m_children;
    UIWidget * m_pFocus;
  };
}

#endif