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
    UIWindow(UIWidget * pParent, vec2 const position, vec2 const & size);
  public:

    static vec2 const s_minSize;

    enum Flags : uint32_t
    {
      Resizable = (1u << 0),
      Movable   = (1u << 1),

      COUNT
    };

    static UIWindow * Create(UIWidget * pParent, vec2 const position, vec2 const & size, uint32_t flags = Resizable | Movable);
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

    void UpdateState(UIWindowState *);

  private:

    UIWindowState *m_pState;
  };
}

#endif