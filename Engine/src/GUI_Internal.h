//@group UI

#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "Utils.h"

namespace Engine
{
  namespace GUI
  {
    // TODO More distinct name for this?
    class Renderer
    {
      static Renderer * s_pInstance;

      Renderer();
      ~Renderer();

    public:

      static bool Init();
      static void Destroy();
      static Renderer * Instance();

      void SetScreenSize(vec2 const &);

      void DrawBox(UIAABB const &, Colour colour);
      //void DrawRoundedBox(UIAABB const &, Colour colour, float radius);

    private:

      class PIMPL;
      PIMPL * m_pimpl;
    };

    bool Intersection(UIAABB const & A,
                      UIAABB const & B,
                      UIAABB & out);

    bool PointInBox(vec2 const & point, UIAABB const & box);
  }
}

#endif