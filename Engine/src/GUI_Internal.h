//@group UI

#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "Utils.h"
#include "IFontAtlas.h"

namespace Engine
{
  enum class TextAlignment
  {
    Min,
    Max,
    Center,
  };

  enum class TextWrapType
  {
    None,
    Letter,
    Word,
    Ellipse
  };

  struct TextOptions
  {
    uint32_t size;
    Colour colour;
    TextAlignment horizontalAlign;
    TextAlignment verticalAlign;
    TextWrapType wrapType;
  };

  namespace GUI
  {
    // TODO More distinct name for this?
    class Renderer
    {
      static Renderer * s_pInstance;

      Renderer();
      ~Renderer();

    public:

      static Dg::ErrorCode Init();
      static void Destroy();
      static Renderer * Instance();

      void SetScreenSize(vec2 const &);
      void DrawText(std::string const &, UIAABB const &, TextOptions const &);
      void DrawText(std::string const &, UIAABB const &, TextOptions const &, Ref<IFontAtlas>, FontID);
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