//@group UI
//
//#ifndef UIBUTTON_H
//#define UIBUTTON_H
//
//#include "UICommon.h"
//#include "core_utils.h"
//#include "UIWidget.h"
//#include "Texture.h"
//
//namespace Engine
//{
//  class UIButton : public UIWidget
//  {
//  public:
//
//    enum State
//    {
//      Default,
//      Hover,
//
//      Count
//    };
//
//    UIButton(mat3 const & Global_to_Local, int depth);
//    ~UIButton();
//
//    UIState HoverOn() override;
//    UIState HoverOff() override;
//    UIState DoMouseDown() override;
//
//    void Draw() override;
//
//    void SetText(std::string const &);
//    void SetFont(uint32_t);
//
//  private:
//
//    SubBlockID m_text;
//  };
//}
//
//#endif