//@group GUI

#include "GUI.h"
#include "GUI_Internal.h"

#define CLR_NORMAL 0xFF94592E
#define CLR_HOVER 0xFFF39621
#define CLR_OUTLINE 0xFF683A0E
#define CLR_TEXT 0xFFF9F0E5

namespace DgE
{
  namespace GUI
  {
    static Style *s_pStyle = nullptr;

    void ResetStyle()
    {
      s_pStyle->contentMargin = 4.0f;
      s_pStyle->outlineWidth = 2.0f;
      s_pStyle->textLineSpacing = 1.0f;
      s_pStyle->textWrap = true;
      s_pStyle->sliderCaretWidth = 16.0f;
      s_pStyle->sliderCaretHeight = 20.0f;
      s_pStyle->sliderBarHeight = 16.0f;

      s_pStyle->colours[col_Text]                   = CLR_TEXT;

      s_pStyle->colours[col_ContainerFace]          = 0xCC333333;
      s_pStyle->colours[col_ContainerOutline]       = 0xCC404040;
      s_pStyle->colours[col_ContainerGrab]          = 0xFF909090;
      s_pStyle->colours[col_ContainerGrabHover]     = 0xFFC0C0C0;

      s_pStyle->colours[col_ButtonFace]             = CLR_NORMAL;
      s_pStyle->colours[col_ButtonText]             = CLR_TEXT;
      s_pStyle->colours[col_ButtonOutline]          = CLR_OUTLINE;
      s_pStyle->colours[col_ButtonFaceHover]        = CLR_HOVER;
      s_pStyle->colours[col_ButtonTextHover]        = CLR_TEXT;
      s_pStyle->colours[col_ButtonOutlineHover]     = CLR_OUTLINE;

      s_pStyle->colours[col_Checkbox]               = 0xFFCBEEFF;
      s_pStyle->colours[col_CheckboxHover]          = 0xFFCBEEFF;
      s_pStyle->colours[col_CheckboxTick]           = 0xFF69CA39;
      s_pStyle->colours[col_CheckboxTickHover]      = 0xFF69CA39;

      s_pStyle->colours[col_SliderLower]             = CLR_NORMAL;
      s_pStyle->colours[col_SliderLowerHover]        = CLR_NORMAL;
      s_pStyle->colours[col_SliderLowerGrab]         = CLR_NORMAL;
      s_pStyle->colours[col_SliderUpper]             = 0xFF909090;
      s_pStyle->colours[col_SliderUpperHover]        = 0xFF909090;
      s_pStyle->colours[col_SliderUpperGrab]         = 0xFF909090;
      s_pStyle->colours[col_SliderCaret]             = CLR_NORMAL;
      s_pStyle->colours[col_SliderCaretHover]        = CLR_HOVER;
      s_pStyle->colours[col_SliderCaretGrab]         = 0xFFF3AA33;
      s_pStyle->colours[col_SliderOutline]           = CLR_OUTLINE;
      s_pStyle->colours[col_SliderOutlineHover]      = CLR_OUTLINE;
      s_pStyle->colours[col_SliderOutlineGrab]       = CLR_OUTLINE;
    }

    Dg::ErrorCode Init()
    {
      Dg::ErrorCode result;

      DG_ERROR_CHECK(Renderer::Init());
      s_pStyle = new Style();
      ResetStyle();

      result = Dg::ErrorCode::None;
    epilogue:
      return result;
    }

    void ShutDown()
    {
      delete s_pStyle;
      s_pStyle = nullptr;

      Renderer::Destroy();
    }

    Style & GetStyle()
    {
      return *s_pStyle;
    }
  }
}