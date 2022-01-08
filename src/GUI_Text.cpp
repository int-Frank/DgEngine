//@group GUI

#include "GUI_Text.h"
#include "GUI.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "Renderer.h"
#include "unicode.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "DgError.h"

#define MAX_TEXTURES 1024

namespace DgE
{
  namespace GUI
  {
    class Text::PIMPL
    {
    public:

      PIMPL()
        : pParent(nullptr)
        , text()
        , style{}
        , aabb{}
      {}

      void HandleMessage(Message_GUI_PointerDown *, Text *pText);
      void HandleMessage(Message_GUI_PointerMove *, Text *pText);

      Widget *pParent;
      std::string text;
      Style::Text style;
      UIAABB aabb;
    };

    enum class ParseState
    {
      NewLine,
      WhiteSpace,
      PersistantWhiteSpace,
      Word,
      BreakableWord
    };

    struct TextContext
    {
      // Constants...
      UIAABB div;
      UIAABB divViewable;
      Style::Text::HorizontalAlignment horizontalAlign;
      Style::Text::VerticalAlignment verticalAlign;
      int16_t lineSpacing;
      uint32_t cpCount;
      int16_t ascent;
      int16_t descent;
      bool wrap;

      // Vars...
      int32_t posX;
      int32_t lineY;

      uint32_t nextLineBegin;
      uint32_t lineBegin;
      uint32_t lineSize;

      uint32_t writtenCPs;

      uint16_t currentTextureID;
    };

    struct CPData
    {
      CodePoint cp;
      GlyphData data;
    };

    // Global buffers to avoid excessive memory allocating to render text
    // TODO This still requires copying to render memory. Perhaps we can write directly to a chunk of Render memory.
    // [x, y, tx, ty, sizex, sizey], ...
    static float s_textVertexBuffer[MAX_TEXT_CHARACTERS * 6] = {};
    static CPData s_glyphData[MAX_TEXT_CHARACTERS] = {};
    static uint16_t s_textureIDs[MAX_TEXTURES] = {};

    static uint32_t InsertTextureID(uint32_t a_count, uint16_t a_id)
    {
      bool found = false;
      uint32_t count = a_count;
      for (uint32_t i = 0; i < count; i++)
      {
        if (a_id == s_textureIDs[i])
        {
          found = true;
          break;
        }
      }
      if (!found && (count < MAX_TEXTURES))
      {
        s_textureIDs[count] = a_id;
        count++;
      }
      return count;
    }

    // Discard control characters
    static bool ShouldDiscard(CodePoint a_cp)
    {
      bool discard = false;
      discard = discard || (a_cp < 0x0A);
      discard = discard || ((a_cp > 0x0A) && (a_cp < 0x20));
      discard = discard || (a_cp == 0x7F);
      discard = discard || ((a_cp >= 0x80) && (a_cp <= 0x9F));
      return discard;
    }

    // Spaces that break only
    static bool IsWhiteSpace(CodePoint a_cp)
    {
      static CodePoint const s_whiteSpaceChar[] =
      {
        0x0020, // SPACE                            Depends on font, typically 1/4 em, often adjusted
        0x1680, // OGHAM SPACE MARK                 Unspecified; usually not really a space but a dash
        0x180E, // MONGOLIAN VOWEL SEPARATOR        0 width
        0x2000, // EN QUAD                          1 en(= 1/2 em)
        0x2001, // EM QUAD                          1 em(nominally, the height of the font)
        0x2002, // EN SPACE(nut)                    1 en(= 1/2 em)
        0x2003, // EM SPACE(mutton)                 1 em
        0x2004, // THREE-PER-EM SPACE(thick space)  1/3 em
        0x2005, // FOUR-PER-EM SPACE(mid space)     1/4 em
        0x2006, // SIX-PER-EM SPACE                 1/6 em
        0x2007, // FIGURE SPACE                     “Tabular width”, the width of digits
        0x2008, // PUNCTUATION SPACE                The width of a period “.”
        0x2009, // THIN SPACE                       1/5 em(or sometimes 1/6 em)
        0x200A, // HAIR SPACE                       Narrower than THIN SPACE
        0x200B, // ZERO WIDTH SPACE                 0 width
        0x205F, // MEDIUM MATHEMATICAL SPACE        4/18 em
        0x3000, // IDEOGRAPHIC SPACE                The width of ideographic(CJK) characters.
      };

      for (size_t i = 0; i < ARRAY_SIZE(s_whiteSpaceChar); i++)
      {
        if (a_cp == s_whiteSpaceChar[i])
          return true;
      }
      return false;
    }

    static bool IsNewLine(CodePoint a_cp)
    {
      return a_cp == 0x0A;
    }

    static bool CPInsideDiv(TextContext const & context, uint32_t index, float offsetX)
    {
      GlyphData * pData = &s_glyphData[index].data;
      vec2 position(float(context.posX) + float(pData->bearingX) + offsetX, 
                    float(context.lineY) - float(pData->bearingY));
      vec2 size(float(pData->width), float(pData->height));
      UIAABB glyphBounds = {position, size};
      UIAABB result = {};
      return Intersection(context.div, glyphBounds, result);
    }

    // Writes character and moves to the next.
    // Assumes will not overflow s_textVertexBuffer.
    static void WriteCharacter(TextContext & context, uint32_t index, float offsetX)
    {
      GlyphData * pData = &s_glyphData[index].data;
      if (CPInsideDiv(context, index, offsetX) && (pData->textureID == context.currentTextureID))
      {
        float x = float(context.posX + pData->bearingX) + offsetX;
        float y = float(context.lineY - pData->bearingY);

        uint32_t ind = context.writtenCPs * 6;

        s_textVertexBuffer[ind + 0] = x;
        s_textVertexBuffer[ind + 1] = y;
        s_textVertexBuffer[ind + 2] = pData->posX;
        s_textVertexBuffer[ind + 3] = pData->posY;
        s_textVertexBuffer[ind + 4] = pData->width;
        s_textVertexBuffer[ind + 5] = pData->height;

        context.writtenCPs++;
      }
      context.posX += int32_t(pData->advance);
    }

    static void AdjustY(TextContext const & context, float offsetY)
    {
      for (uint32_t i = 0; i < context.writtenCPs; i++)
        s_textVertexBuffer[i * 6 + 1] += offsetY;
    }

    static void WriteBlock(TextContext & context, float offsetX)
    {
      for (uint32_t i = context.lineBegin; i < (context.lineBegin + context.lineSize); i++)
        WriteCharacter(context, i, offsetX);
    }

    static bool LineTooLong(TextContext const & context, int32_t lineLen)
    {
      return (context.wrap && float(lineLen) > context.div.size.x());
    }

    static bool GetNextLine(TextContext & context)
    {
      if (context.nextLineBegin >= context.cpCount)
        return false;

      context.lineBegin = context.nextLineBegin;
      uint32_t lineEnd = context.nextLineBegin;
      uint32_t pos = context.nextLineBegin;
      ParseState state = ParseState::NewLine;

      bool done = false;

      for (; pos < context.cpCount; pos++)
      {
        CPData const & data = s_glyphData[pos];

        switch (state)
        {
          case ParseState::NewLine:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (IsWhiteSpace(data.cp))
            {
              state = ParseState::PersistantWhiteSpace;
            }
            else
            {
              lineEnd = pos + 1;
              state = ParseState::Word;
            }
            break;
          }

          case ParseState::PersistantWhiteSpace:
          {
            context.nextLineBegin = pos;
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin++;
              done = true;
            }
            else if (!IsWhiteSpace(data.cp))
            {
              lineEnd = pos + 1;
              state = ParseState::Word;
            }
            break;
          }

          case ParseState::WhiteSpace:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (!IsWhiteSpace(data.cp))
            {
              lineEnd = pos + 1;
              state = ParseState::Word;
            }
            break;
          }

          case ParseState::Word:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (IsWhiteSpace(data.cp))
            {
              lineEnd = pos;
              state = ParseState::WhiteSpace;
            }
            else
            {
              lineEnd = pos + 1;
            }
            break;
          }

          default:
          {
            DG_ASSERT(false, "Malformed state machine!");
          }
        }

        if (done)
          break;
      }

      if (pos == context.cpCount)
        context.nextLineBegin = context.cpCount;

      context.lineSize = lineEnd - context.lineBegin;
      return true;
    }

    // Returns true if more lines to process
    static bool GetNextLineWrap(TextContext & context)
    {
      if (context.nextLineBegin >= context.cpCount)
        return false;

      int32_t lineLength = 0;
      context.lineBegin = context.nextLineBegin;
      uint32_t lineEnd = context.nextLineBegin;
      uint32_t lineEndBkup = context.nextLineBegin;
      uint32_t pos = context.nextLineBegin;
      ParseState state = ParseState::NewLine;

      bool done = false;

      for (;pos < context.cpCount; pos++)
      {
        CPData const & data = s_glyphData[pos];
        lineLength += data.data.advance;

        switch (state)
        {
          case ParseState::NewLine:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (IsWhiteSpace(data.cp))
            {
              state = ParseState::PersistantWhiteSpace;
            }
            else
            {
              lineEnd = pos + 1;
              state = ParseState::BreakableWord;
            }
            break;
          }

          case ParseState::WhiteSpace:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (!IsWhiteSpace(data.cp))
            {
              context.nextLineBegin = pos;
              if (LineTooLong(context, lineLength))
              {
                done = true;
              }
              else
              {
                lineEnd = pos + 1;
                state = ParseState::Word;
              }
            }
            break;
          }

          case ParseState::PersistantWhiteSpace:
          {
            context.nextLineBegin = pos;
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin++;
              done = true;
            }
            else if (LineTooLong(context, lineLength))
            {
              done = true;
            }
            else if (!IsWhiteSpace(data.cp))
            {
              lineEnd = pos + 1;
              state = ParseState::Word;
            }
            break;
          }

          case ParseState::Word:
          {
            if (IsNewLine(data.cp))
            {
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (IsWhiteSpace(data.cp))
            {
              lineEnd = pos;
              lineEndBkup = pos;
              state = ParseState::WhiteSpace;
            }
            else if (LineTooLong(context, lineLength))
            {
              lineEnd = lineEndBkup;
              done = true;
            }
            else
            {
              lineEnd = pos + 1;
            }
            break;
          }

          case ParseState::BreakableWord:
          {
            if (IsNewLine(data.cp))
            {
              lineEnd = pos;
              context.nextLineBegin = pos + 1;
              done = true;
            }
            else if (IsWhiteSpace(data.cp))
            {
              lineEnd = pos;
              lineEndBkup = pos;
              state = ParseState::WhiteSpace;
            }
            else if (LineTooLong(context, lineLength))
            {
              lineEnd = pos;
              context.nextLineBegin = pos;
              done = true;
            }
            else
            {
              lineEnd = pos + 1;
            }
            break;
          }
        }

        if (done)
          break;
      }

      if (pos == context.cpCount)
        context.nextLineBegin = context.cpCount;

      context.lineSize = lineEnd - context.lineBegin;
      return true;
    }

    static uint32_t LineLength(TextContext const & context)
    {
      if (context.lineSize == 0)
        return 0;

      uint32_t result = 0;
      for (uint32_t i = context.lineBegin; (i + 1) < (context.lineBegin + context.lineSize); i++)
        result += s_glyphData[i].data.advance;

      GlyphData * pData = &s_glyphData[context.lineBegin + context.lineSize - 1].data;
      result += (pData->bearingX + pData->width);
      return result;
    }

    static void WriteText(TextContext & context)
    {
      context.nextLineBegin = 0;
      context.writtenCPs = 0;

      int32_t line = 0;
      int32_t posX = context.posX;
      int32_t lineY = context.lineY;

      while (true)
      {
        bool lineRead = false;
        if (context.wrap)
          lineRead = GetNextLineWrap(context);
        else
          lineRead = GetNextLine(context);

        if (!lineRead)
          break;

        float offsetX = 0.0f;
        if (context.horizontalAlign == Style::Text::HorizontalAlignment::Right)
          offsetX = context.div.size.x() - (float)LineLength(context);
        else if (context.horizontalAlign == Style::Text::HorizontalAlignment::Centre)
          offsetX = (context.div.size.x() - (float)LineLength(context)) / 2.0f;

        offsetX = round(offsetX);

        WriteBlock(context, offsetX);
        line++;
        context.posX = posX;
        context.lineY = lineY + line * context.lineSpacing;
        if ((context.lineY - context.ascent) > (context.div.position.y() + context.div.size.y()))
          break;
      }

      if (context.verticalAlign == Style::Text::VerticalAlignment::Centre)
      {
        float ySize = (float)context.lineSpacing * line;
        if (ySize < context.div.size.y())
          AdjustY(context, round((context.div.size.y() - ySize) / 2.0f));
      }
    }

    static uint32_t DecodeText(std::string const & a_str, uint32_t & a_textureCount, uint32_t a_size)
    {
      uint32_t count = 0;
      uint32_t countRenderable = 0;
      UTF8Parser parser(a_str.c_str());
      a_textureCount = 0;

      while (!parser.Done() && (countRenderable < MAX_TEXT_CHARACTERS))
      {
        CodePoint cp = parser.Next();
        if (cp != INVALID_CHAR)
        {
          if (ShouldDiscard(cp))
            continue;

          if (IsNewLine(cp))
          {
            s_glyphData[count].cp = cp;
          }
          else
          {
            GlyphData * pData = Renderer::GetGlyphData(cp, a_size);
            if (pData == nullptr)
              pData = Renderer::GetGlyphData(uint32_t('?'), a_size);
            DG_ASSERT(pData != nullptr, "Default font atlas missing '?' character");
            s_glyphData[count].cp = cp;
            s_glyphData[count].data = *pData;
            if (pData->textureID != INVALID_FONT_TEXTURE)
            {
              a_textureCount = InsertTextureID(a_textureCount, pData->textureID);
              countRenderable++;
            }
          }
          count++;
        }
      }

      return count;
    }

    Text::Text(std::string const & text, vec2 const & position, vec2 const & size, Style::Text const &style, std::initializer_list<WidgetFlag> flags)
      : Widget({WidgetFlag::NotResponsive,
                WidgetFlag::StretchHeight,
                WidgetFlag::StretchWidth}, flags)
      , m_pimpl(new PIMPL())
    {
      SetStyle(style);
      m_pimpl->aabb = {position, size};
      m_pimpl->text = text;
    }

    Text *Text::Create(std::string const &text, vec2 const &position, vec2 const &size, Style::Text const &style, std::initializer_list<WidgetFlag> flags)
    {
      return new Text(text, position, size, style, flags);
    }

    Text *Text::Create(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags)
    {
      return new Text(text, position, size, s_style, flags);
    }

    Text::~Text()
    {

    }

    void Text::SetText(std::string const & a_str)
    {
      m_pimpl->text = a_str;
    }

    Style::Text const &Text::GetDefaultStyle()
    {
      return s_style;
    }

    Style::Text const &Text::GetStyle() const
    {
      return m_pimpl->style;
    }

    void Text::SetStyle(Style::Text const &style)
    {
      m_pimpl->style = style;
    }

    void Text::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalViewableArea(viewableWindow))
        return;

      uint32_t textureCount;

      TextContext context;
      context.div = {GetGlobalPosition(), GetSize()};
      Renderer::GetCharacterSizeRange(m_pimpl->style.size, context.ascent, context.descent);

      context.wrap = m_pimpl->style.wrapText;
      context.divViewable = viewableWindow;
      context.horizontalAlign = m_pimpl->style.horizontalAlign;
      context.verticalAlign = m_pimpl->style.verticalAlign;
      context.lineSpacing = int16_t(m_pimpl->style.lineSpacing * (context.ascent - context.descent));
      context.cpCount = DecodeText(m_pimpl->text, textureCount, m_pimpl->style.size);
      
      ::DgE::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      
      for (uint32_t i = 0; i < textureCount; i++)
      {
        context.posX = int16_t(context.div.position.x());
        context.lineY = int16_t(context.div.position.y()) + context.ascent;
        context.currentTextureID = s_textureIDs[i];

        WriteText(context);

        Renderer::DrawText(context.currentTextureID, m_pimpl->style.colourText, context.writtenCPs, s_textVertexBuffer);
      }
    }
    
    void Text::SetGlyphSize(uint32_t a_size)
    {
      m_pimpl->style.size = a_size;
    }

    WidgetState Text::QueryState() const
    {
      return WidgetState::None;
    }

    Widget * Text::GetParent() const
    {
      return m_pimpl->pParent;
    }

    void Text::SetParent(Widget * a_pParent)
    {
      m_pimpl->pParent = a_pParent;
    }

    void Text::_HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg), this);
      else if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(a_pMsg), this);
    }

    void Text::PIMPL::HandleMessage(Message_GUI_PointerDown * a_pMsg, Text *pText)
    {
      UIAABB aabb;
      if (!pText->GetGlobalViewableArea(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->SetFlag(DgE::Message::Flag::Handled, true);
    }

    void Text::PIMPL::HandleMessage(Message_GUI_PointerMove * a_pMsg, Text *pText)
    {
      UIAABB aabb;
      if (!pText->GetGlobalViewableArea(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->ConsumeHover();
    }

    void Text::_SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->aabb.position = a_pos;
    }

    void Text::_SetSize(vec2 const & a_size)
    {
      m_pimpl->aabb.size = a_size;
    }

    vec2 Text::_GetLocalPosition()
    {
      return m_pimpl->aabb.position;
    }

    vec2 Text::_GetSize()
    {
      return m_pimpl->aabb.size;
    }
  }
}