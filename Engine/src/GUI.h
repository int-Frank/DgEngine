//@group GUI

#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include "Utils.h"
#include "DgError.h"

#define GUI_FONT_SIZE 16
#define GUI_FONT_SIZE_GRAB 16
#define GUI_FONT_SIZE_BOX 32
#define GUI_FONT_SIZE_TICK 22
#define GUI_FONT_SIZE_RADIO 32

namespace Engine
{
  namespace GUI
  {
    enum StyleColour
    {
      col_Text,

      col_ContainerFace,
      col_ContainerOutline,
      col_ContainerGrab,
      col_ContainerGrabHover,

      col_ButtonFace,
      col_ButtonText,
      col_ButtonOutline,
      col_ButtonFaceHover,
      col_ButtonTextHover,
      col_ButtonOutlineHover,

      col_Checkbox,
      col_CheckboxHover,
      col_CheckboxTick,
      col_CheckboxTickHover,

      col_SliderLower,
      col_SliderLowerHover,
      col_SliderLowerGrab,
      col_SliderUpper,
      col_SliderUpperHover,
      col_SliderUpperGrab,
      col_SliderCaret,
      col_SliderCaretHover,
      col_SliderCaretGrab,
      col_SliderOutline,
      col_SliderOutlineHover,
      col_SliderOutlineGrab,

      col_COUNT
    };

    struct Style
    {
      Colour colours[col_COUNT];
      float contentMargin;
      float outlineWidth;
      float textLineSpacing;
      bool textWrap;
      float sliderCaretWidth;
      float sliderCaretHeight;
      float sliderBarHeight;
    };

    Dg::ErrorCode Init();
    void ShutDown();

    Style & GetStyle();
    void ResetStyle();
  }
}

#endif