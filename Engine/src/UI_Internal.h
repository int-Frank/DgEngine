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

    void DrawBox(vec2 const & position, vec2 const & size, Colour colour);
    void DrawRoundedBox(vec2 const & position, vec2 const & size, Colour colour, float radius);

  private:

    class PIMPL;
    PIMPL * m_pimpl;
  };
}

#endif