//@group GUI

#ifndef GUI_TEXT_H
#define GUI_TEXT_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace DgE
{
  namespace GUI
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

    struct TextAttributes
    {
      Colour colourText;
      HorizontalAlignment horizontalAlign;
      VerticalAlignment verticalAlign;
      float lineSpacing;
      bool wrapText;

      uint32_t size; // Internal use for now
    };

    class Text : public Widget
    {
      Text(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags);
    public:

      static Text * Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const *pAttrs = nullptr, std::initializer_list<WidgetFlag> flags ={});

      ~Text();

      void SetWrap(bool);
      void SetText(std::string const &);
      void SetColour(Colour);

      //void SetFont(FontID fontID, uint32_t size);
      void SetGlyphSize(uint32_t size);
      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

    private:

      void _HandleMessage(Message *) override;

      void HandleMessage(Message_GUI_PointerDown *);
      void HandleMessage(Message_GUI_PointerMove *);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      Widget * m_pParent;
      std::string m_text;
      TextAttributes m_attributes;
      UIAABB m_aabb;
    };
  }
}

#endif