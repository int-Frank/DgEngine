//@group UI

#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "UICommon.h"

namespace Engine
{
  class UIRenderer
  {
    static UIRenderer * s_pInstance;

    UIRenderer();
    ~UIRenderer();

  public:

    static bool Init();
    static void Destroy();
    static UIRenderer * Instance();

    void SetScreenSize(vec2 const &);

    void DrawBox(UIAABB const &, Colour colour);
    //void DrawCorner(UIAABB const &, Colour colour, UIAABB const * pSissor = nullptr);
    //void DrawRoundedBox(UIAABB const &, Colour colour, float radius, UIAABB const * pSissor = nullptr);

  private:

    class PIMPL;
    PIMPL * m_pimpl;
  };

  bool UIIntersection(UIAABB const & A,
                      UIAABB const & B,
                      UIAABB & out);
}

#endif