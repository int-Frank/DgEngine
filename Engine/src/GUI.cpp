//@group GUI

#include "GUI.h"
#include "GUI_Internal.h"

namespace Engine
{
  namespace GUI
  {
    static Style *s_pStyle = nullptr;

    void ResetStyle()
    {
      s_pStyle->contentBorder = 4.0f;
      s_pStyle->textLineSpacing = 1.0f;
      s_pStyle->textWrap = true;

      s_pStyle->colours[col_Text]                 = 0xFFFFFFFF;
      s_pStyle->colours[col_ContainerBackground]  = 0x33FFFFFF;
      s_pStyle->colours[col_ContainerGrab]        = 0xFF00FFFF;
      s_pStyle->colours[col_ContainerGrabHover]   = 0xFF00FF00;
      s_pStyle->colours[col_Button]               = 0xFFFFFF00;
      s_pStyle->colours[col_ButtonHover]          = 0xFFFF0000;
      s_pStyle->colours[col_ButtonText]           = 0xFF888888;
      s_pStyle->colours[col_ButtonTextHover]      = 0xFF888888;
      s_pStyle->colours[col_CheckboxBox]          = 0xFF444444;
      s_pStyle->colours[col_CheckboxBoxHover]     = 0xFF888888;
      s_pStyle->colours[col_CheckboxTick]         = 0xFF00FF00;
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