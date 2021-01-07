//@group GUI

#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include "Utils.h"
#include "DgError.h"

#define GUI_FONT_SIZE 16
#define GUI_FONT_SIZE_GRAB 16
#define GUI_FONT_SIZE_BOX 32
#define GUI_FONT_SIZE_TICK 24
#define GUI_FONT_SIZE_RADIO 32

namespace Engine
{
  namespace GUI
  {
    enum StyleColour
    {
      col_Text,
      col_ContainerBackground,
      col_ContainerGrab,
      col_ContainerGrabHover,
      col_Button,
      col_ButtonHover,
      col_ButtonText,
      col_ButtonTextHover,
      col_CheckboxBox,
      col_CheckboxBoxHover,
      col_CheckboxTick,

      col_COUNT
    };

    struct Style
    {
      Colour colours[col_COUNT];

      float contentBorder;
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