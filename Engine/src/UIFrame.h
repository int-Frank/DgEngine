//@group UI

#ifndef UIFORM_H
#define UIFORM_H

#include "MessageHandler.h"
#include "UICommon.h"
#include "DgDoublyLinkedList.h"
#include "EngineMessages.h"

namespace Engine
{
  class UIWidget;

  class UIFrame : public MessageHandler
  {
  public:
    class CanvasState;
  public:

    UIFrame();
    ~UIFrame();

    void HandleMessage(Message *);

    void Draw();

    void Clear();
    void Add(UIWidget *);

  private:

    void HandleMessage(Message_Window_Resized *);
    void HandleMessage(Message_GUI_Up *);
    void HandleMessage(Message_GUI_Down *);
    void HandleMessage(Message_GUI_Left *);
    void HandleMessage(Message_GUI_Right *);
    void HandleMessage(Message_GUI_PointerUp *);
    void HandleMessage(Message_GUI_PointerDown *);
    void HandleMessage(Message_GUI_PointerMove *);
    void HandleMessage(Message_GUI_Text *);

  private:

    Dg::DoublyLinkedList<UIWidget *> m_widgets;
    UIWidget * m_pFocus;
  };
}

#endif