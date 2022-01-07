//@group GUI

#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include "Utils.h"
#include "DgError.h"

#define GUI_FONT_SIZE 16
#define GUI_FONT_SIZE_GRAB 16
#define GUI_FONT_SIZE_BOX 32
#define GUI_FONT_SIZE_TICK 13
#define GUI_FONT_SIZE_RADIO 32

namespace DgE
{
  namespace GUI
  {
    namespace Style
    {
      struct Text
      {
        enum class HorizontalAlignment
        {
          Left,
          Centre,
          Right
        };

        enum class VerticalAlignment
        {
          Top,
          Centre
        };

        Colour colourText;
        HorizontalAlignment horizontalAlign;
        VerticalAlignment verticalAlign;
        float lineSpacing;
        bool wrapText;

        uint32_t size; // Internal use for now
      };

      struct Slider
      {
        enum State
        {
          s_Default,
          s_Hover,
          s_Grab,
          s_COUNT
        };

        enum Component
        {
          c_LowerBar,
          c_UpperBar,
          c_Caret,
          c_COUNT
        };

        struct ComponentData
        {
          float height;
          float borderWidth;
        };

        struct Colours
        {
          Colour face;
          Colour border;
        };

        ComponentData components[c_COUNT];
        Colours colours[c_COUNT][s_COUNT];
        float caretWidth;
      };

      struct Window
      {
        Colour face;
        Colour border;
        Colour grabButton;
        Colour grabButtonHover;
        float borderWidth;
        Slider scrollBars;
      };

      struct Button
      {
        enum State
        {
          Default,
          Hover,
          COUNT
        };

        struct Colours
        {
          Colour face;
          Colour border;
        };

        Colours colours[COUNT];
        Text text[COUNT];
        float borderWidth;
        float contentMargin;
      };

      struct Checkbox
      {
        enum State
        {
          Default,
          Hover,
          COUNT
        };

        struct Colours
        {
          Colour box;
          Colour border;
        };

        Colours colours[COUNT];
        Colour tick;
      };
    };

    Dg::ErrorCode Init();
    void ShutDown();
  }
}

#endif