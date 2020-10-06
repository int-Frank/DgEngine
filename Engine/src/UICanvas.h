//@group UI
//
//#ifndef UICANVAS_H
//#define UICANVAS_H
//
//#include "UICommon.h"
//#include "DgDoublyLinkedList.h"
//#include "InputCodes.h"
//#include "DgBinPacker.h"
//#include "DgSlotMap.h"
//
//namespace Engine
//{
//  class UIWidget;
//
//  class UICanvas
//  {
//  public:
//    class CanvasState;
//  public:
//
//    UICanvas();
//    ~UICanvas();
//
//    void DoMouseMove(vec2 const &);
//    void DoMouseDown(vec2 const &);
//    void DoMouseUp(vec2 const &);
//    void DoScroll(float x);
//    void DoKeyDown(InputCode);
//    void DoKeyUp(InputCode);
//    void DoText(char text[32]);
//    void DoActivate();
//
//    void Draw();
//
//    void Clear();
//    void Add(UIWidget *);
//
//  private:
//
//    void ChangeState(CanvasState *);
//
//  private:
//
//    Dg::DoublyLinkedList<UIWidget*> m_widgets; 
//    CanvasState *m_pState;
//  };
//}
//
//#endif