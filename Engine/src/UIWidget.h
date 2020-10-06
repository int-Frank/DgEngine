//@group UI
//
//#ifndef UIWIDGET_H
//#define UIWIDGET_H
//
//#include "core_utils.h"
//#include "InputCodes.h"
//#include <string>
//
///*
//  - Button
//  - Check box
//  - Radio button
//  - Slider
//  - Drop down list
//  - Scroll list
//  - Image
//    - Text (maybe derives from image)
//  - Text Input
//  - Text display
//  - Text window (logging window)
//*/
//
//namespace Engine
//{
//  struct UIData
//  {
//    union
//    {
//      bool bVal;
//      int iVal;
//      float fVal;
//      char* pcVal;
//    };
//  };
//
//  typedef void (*fnUICallback)(UIData const &);
//
//  enum class UICallback
//  {
//    HoverOn,
//    HoverOff,
//    GainFocus,
//    LoseFocus,
//    Activate,   //Button
//    NewValue,   //Slider, Lists
//    Toggle,     //Checkbox
//    COUNT
//  };
//
//  enum class UIState
//  {
//    None,
//    HoverOn,
//    Active // Eg text inputs in focus and waiting for input
//  };
//
//  class UIWidget
//  {
//  public:
//
//    UIWidget(mat3 const & Global_to_Local, int depth);
//
//    virtual ~UIWidget();
//    
//    void SetTransform(mat3 const & trans);
//    void SetDepth(int);
//
//    int GetDepth() const;
//
//    virtual void Draw() {}
//
//    virtual bool IsInside(vec2 const &point);
//
//    virtual UIState HoverOn() {}
//    virtual UIState HoverOff() {}
//    virtual UIState FocusOn() {}
//    virtual UIState FocusOff() {}
//    virtual UIState DoKeyDown(InputCode) {}
//    virtual UIState DoKeyUp(InputCode) {}
//    virtual UIState DoMouseDown() {}
//    virtual UIState DoMouseUp() {}
//    virtual UIState DoScroll(float val) {}
//    virtual UIState DoText(char text[32]) {}
//    virtual UIState Activate() {} // Enter key, most widgets won't have this
//    UIState QueryState() {} 
//
//    void SetCallback(UICallback, fnUICallback);
//
//  protected:
//
//    virtual void NewTransform() {}
//
//  protected:
//
//    fnUICallback m_bindings[static_cast<size_t>(UICallback::COUNT)];
//    mat3 m_T_Global_to_Local;
//    int m_depth;
//    UIState m_state;
//  };
//}
//
//#endif