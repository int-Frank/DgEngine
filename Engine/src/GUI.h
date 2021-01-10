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

      col_COUNT
    };

    struct Style
    {
      Colour colours[col_COUNT];
      float contentMargin;
      float outlineWidth;
      float textLineSpacing;
      bool textWrap;
    };

    Dg::ErrorCode Init();
    void ShutDown();

    Style & GetStyle();
    void ResetStyle();
  }
}

#endif