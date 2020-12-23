//@group GUI

#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    enum class TextAlignment
    {
      Min,    // left/top
      Max,    // right/bottom
      Center,
    };

    enum class TextWrapType
    {
      None,
      Letter,
      Word,
      Ellipse
    };

    struct TextAttributes
    {
      uint32_t size;
      Colour colourText;
      Colour colourBackground;
      TextAlignment horizontalAlign;
      TextAlignment verticalAlign;
      TextWrapType wrapType;
    };

    class Text : public Widget
    {
      Text(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags);
    public:

      static Text * Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const *pAttrs = nullptr, std::initializer_list<WidgetFlag> flags ={});

      ~Text();

      void SetBorder(float);
      void SetText(std::string const &);

      void HandleMessage(Message *) override;

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

    private:

      void HandleMessage(Message_GUI_PointerDown *);
      void HandleMessage(Message_GUI_PointerMove *);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      Widget * m_pParent;
      std::string m_text;
      float m_border;
      TextAttributes m_attributes;
      UIAABB m_aabb;
    };
  }
}

#endif