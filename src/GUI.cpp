//@group GUI

#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Text.h"
#include "GUI_Button.h"
#include "GUI_Slider.h"
#include "GUI_Window.h"
#include "GUI_Checkbox.h"

#define BORDER_WIDTH 1.0f
#define NO_BORDER 0.0f

#define CLR_NONE 0

#define CLR_BACKGROUND          0xEE121212
#define CLR_PRIMARY             0xFF0088FD
#define CLR_PRIMARY_ACCENT      0xFF36A1FD
#define CLR_PRIMARY_LIGHT       0xFF7FCAFF

#define CLR_SECONDARY_DARK      0xFF535250
#define CLR_SECONDARY_SEMIDARK  0xFF535250
#define CLR_SECONDARY_SEMILIGHT 0xFFBCBBBB
#define CLR_SECONDARY_LIGHT     0xFFE1E1E1

#define CLR_ACCENT              0xFF727e15
#define CLR_ACCENT_LIGHT        0xFF8C9C1A

//--------------------------------------------------------------
// Text
//--------------------------------------------------------------

#define CLR_TEXT_ON_BACKGROUND  0xFFDADCDA
#define CLR_TEXT_ON_PRIMARY     0xFF141918

//--------------------------------------------------------------
// Slider
//--------------------------------------------------------------

#define CLR_SLIDER_DEFAULT_UPPER          CLR_PRIMARY_LIGHT
#define CLR_SLIDER_DEFAULT_LOWER          CLR_PRIMARY
#define CLR_SLIDER_DEFAULT_CARET          CLR_ACCENT
#define CLR_SLIDER_DEFAULT_UPPER_BORDER   CLR_NONE
#define CLR_SLIDER_DEFAULT_LOWER_BORDER   CLR_NONE
#define CLR_SLIDER_DEFAULT_CARET_BORDER   CLR_NONE

#define CLR_SLIDER_HOVER_UPPER            CLR_PRIMARY_LIGHT
#define CLR_SLIDER_HOVER_LOWER            CLR_PRIMARY
#define CLR_SLIDER_HOVER_CARET            CLR_ACCENT_LIGHT
#define CLR_SLIDER_HOVER_UPPER_BORDER     CLR_NONE
#define CLR_SLIDER_HOVER_LOWER_BORDER     CLR_NONE
#define CLR_SLIDER_HOVER_CARET_BORDER     CLR_NONE

#define CLR_SLIDER_GRAB_UPPER             CLR_PRIMARY_LIGHT
#define CLR_SLIDER_GRAB_LOWER             CLR_PRIMARY
#define CLR_SLIDER_GRAB_CARET             CLR_ACCENT_LIGHT
#define CLR_SLIDER_GRAB_UPPER_BORDER      CLR_NONE
#define CLR_SLIDER_GRAB_LOWER_BORDER      CLR_NONE
#define CLR_SLIDER_GRAB_CARET_BORDER      CLR_NONE

//--------------------------------------------------------------
// Window
//--------------------------------------------------------------

#define CLR_CONTAINER_FACE              CLR_BACKGROUND
#define CLR_CONTAINER_BORDER            CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_GRABBUTTON        CLR_PRIMARY
#define CLR_CONTAINER_GRABBUTTON_HOVER  CLR_PRIMARY_ACCENT

#define CLR_CONTAINER_SLIDER_DEFAULT_UPPER          CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_DEFAULT_LOWER          CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_DEFAULT_CARET          CLR_PRIMARY
#define CLR_CONTAINER_SLIDER_DEFAULT_UPPER_BORDER   CLR_NONE
#define CLR_CONTAINER_SLIDER_DEFAULT_LOWER_BORDER   CLR_NONE
#define CLR_CONTAINER_SLIDER_DEFAULT_CARET_BORDER   CLR_NONE

#define CLR_CONTAINER_SLIDER_HOVER_UPPER            CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_HOVER_LOWER            CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_HOVER_CARET            CLR_PRIMARY_ACCENT
#define CLR_CONTAINER_SLIDER_HOVER_UPPER_BORDER     CLR_NONE
#define CLR_CONTAINER_SLIDER_HOVER_LOWER_BORDER     CLR_NONE
#define CLR_CONTAINER_SLIDER_HOVER_CARET_BORDER     CLR_NONE

#define CLR_CONTAINER_SLIDER_GRAB_UPPER             CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_GRAB_LOWER             CLR_SECONDARY_SEMIDARK
#define CLR_CONTAINER_SLIDER_GRAB_CARET             CLR_PRIMARY_ACCENT
#define CLR_CONTAINER_SLIDER_GRAB_UPPER_BORDER      CLR_NONE
#define CLR_CONTAINER_SLIDER_GRAB_LOWER_BORDER      CLR_NONE
#define CLR_CONTAINER_SLIDER_GRAB_CARET_BORDER      CLR_NONE

//--------------------------------------------------------------
// Button
//--------------------------------------------------------------

#define CLR_BUTTON_DEFAULT_FACE   CLR_PRIMARY
#define CLR_BUTTON_DEFAULT_BORDER CLR_NONE
#define CLR_BUTTON_HOVER_FACE     CLR_PRIMARY_ACCENT
#define CLR_BUTTON_HOVER_BORDER   CLR_NONE

//--------------------------------------------------------------
// Checkbox
//--------------------------------------------------------------

#define CLR_CHECKBOX_DEFAULT_BOX    CLR_PRIMARY
#define CLR_CHECKBOX_DEFAULT_BORDER CLR_SECONDARY_LIGHT
#define CLR_CHECKBOX_HOVER_BOX      CLR_PRIMARY
#define CLR_CHECKBOX_HOVER_BORDER   CLR_SECONDARY_LIGHT
#define CLR_CHECKBOX_TICK           CLR_ACCENT

namespace DgE
{
  namespace GUI
  {
    Style::Text const Text::s_style = 
    {
      CLR_TEXT_ON_BACKGROUND,
      Style::Text::HorizontalAlignment::Left,
      Style::Text::VerticalAlignment::Top,
      1.0f,
      true
    };

    Style::Slider const SliderBase::s_style =
    {
      {
        {8.0f, NO_BORDER},
        {4.0f, NO_BORDER},
        {16.0f, NO_BORDER}
      },
      {
        {
          {CLR_SLIDER_DEFAULT_LOWER, CLR_SLIDER_DEFAULT_LOWER_BORDER},
          {CLR_SLIDER_HOVER_LOWER, CLR_SLIDER_HOVER_LOWER_BORDER},
          {CLR_SLIDER_GRAB_LOWER, CLR_SLIDER_GRAB_LOWER_BORDER}
        },
        {
          {CLR_SLIDER_DEFAULT_UPPER, CLR_SLIDER_DEFAULT_UPPER_BORDER},
          {CLR_SLIDER_HOVER_UPPER, CLR_SLIDER_HOVER_UPPER_BORDER},
          {CLR_SLIDER_GRAB_UPPER, CLR_SLIDER_GRAB_UPPER_BORDER}
        },
        {
          {CLR_SLIDER_DEFAULT_CARET, CLR_SLIDER_DEFAULT_CARET_BORDER},
          {CLR_SLIDER_HOVER_CARET, CLR_SLIDER_HOVER_CARET_BORDER},
          {CLR_SLIDER_GRAB_CARET, CLR_SLIDER_GRAB_CARET_BORDER}
        }
      },
      12.0f
    };

    Style::Window const Window::s_style =
    {
      CLR_CONTAINER_FACE, 
      CLR_CONTAINER_BORDER,
      CLR_CONTAINER_GRABBUTTON,
      CLR_CONTAINER_GRABBUTTON_HOVER,
      BORDER_WIDTH,
      {
        {
          {14.0f, NO_BORDER},
          {14.0f, NO_BORDER},
          {10.0f, NO_BORDER}
        },
        {
          {
          {CLR_CONTAINER_SLIDER_DEFAULT_LOWER, CLR_CONTAINER_SLIDER_DEFAULT_LOWER_BORDER},
          {CLR_CONTAINER_SLIDER_HOVER_LOWER, CLR_CONTAINER_SLIDER_HOVER_LOWER_BORDER},
          {CLR_CONTAINER_SLIDER_GRAB_LOWER, CLR_CONTAINER_SLIDER_GRAB_LOWER_BORDER}
        },
        {
          {CLR_CONTAINER_SLIDER_DEFAULT_UPPER, CLR_CONTAINER_SLIDER_DEFAULT_UPPER_BORDER},
          {CLR_CONTAINER_SLIDER_HOVER_UPPER, CLR_CONTAINER_SLIDER_HOVER_UPPER_BORDER},
          {CLR_CONTAINER_SLIDER_GRAB_UPPER, CLR_CONTAINER_SLIDER_GRAB_UPPER_BORDER}
        },
        {
          {CLR_CONTAINER_SLIDER_DEFAULT_CARET, CLR_CONTAINER_SLIDER_DEFAULT_CARET_BORDER},
          {CLR_CONTAINER_SLIDER_HOVER_CARET, CLR_CONTAINER_SLIDER_HOVER_CARET_BORDER},
          {CLR_CONTAINER_SLIDER_GRAB_CARET, CLR_CONTAINER_SLIDER_GRAB_CARET_BORDER}
        }
        },
        12.0f
      }
    };

    Style::Button const Button::s_style =
    {
      {
        {CLR_BUTTON_DEFAULT_FACE, CLR_BUTTON_DEFAULT_BORDER},
        {CLR_BUTTON_HOVER_FACE, CLR_BUTTON_HOVER_BORDER}
      },
      {
        {
          CLR_TEXT_ON_PRIMARY,
          Style::Text::HorizontalAlignment::Centre,
          Style::Text::VerticalAlignment::Centre,
          1.0f,
          true
        },
        {
          CLR_TEXT_ON_PRIMARY,
          Style::Text::HorizontalAlignment::Centre,
          Style::Text::VerticalAlignment::Centre,
          1.0f,
          true
        }
      },
      BORDER_WIDTH,
      { 0.0f, 0.0f }
    };

    Style::Checkbox const Checkbox::s_style =
    {
      {
        {CLR_CHECKBOX_DEFAULT_BOX, CLR_CHECKBOX_DEFAULT_BORDER},
        {CLR_CHECKBOX_HOVER_BOX, CLR_CHECKBOX_HOVER_BORDER}
      },
      CLR_CHECKBOX_TICK
    };

    Dg::ErrorCode Init()
    {
      Dg::ErrorCode result;

      DG_ERROR_CHECK(Renderer::Init());

      result = Dg::ErrorCode::None;
    epilogue:
      return result;
    }

    void ShutDown()
    {
      Renderer::Destroy();
    }
  }
}