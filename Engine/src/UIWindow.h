//@group UI

#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "core_utils.h"
#include "UIWidget.h"
#include "DgDoublyLinkedList.h"

namespace Engine
{
  class UIWindowState;

  class UIWindow : public UIWidget
  {
  public:

    UIWindow(UIWidget * pParent, vec2 const position, vec2 const & size);
    ~UIWindow();

    //void SetPosition(vec2 const &);
    //void SetSize(vec2 const &);
    //void SetBackgroundColour(Colour);
    //void SetHoverOnBackgroundColour(Colour);

    void Clear();
    void Add(UIWidget *);
    void Remove(UIWidget *);

    void HandleMessage(Message *);

    void Draw() override;

    UIState QueryState() const override;
    UIWidget * GetParent() const override;
    void SetParent(UIWidget *) override;

  private:

    UIWindowState *m_pState;
  };
}

#endif