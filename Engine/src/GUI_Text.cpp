//@group GUI

#include <limits>

#include "GUI_Text.h"
#include "Options.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "Renderer.h"
#include "unicode.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "DgError.h"

#define MAX_TEXTURES 1024

namespace Engine
{
  namespace GUI
  {
    struct TextContext
    {
      // Constants...
      UIAABB div;
      UIAABB divViewable;
      TextAlignment horizontalAlign;
      int16_t lineSpacing;
      uint32_t cpCount;

      // Vars...
      int32_t posX;
      int32_t lineY;

      uint32_t cpCurrentPosition;
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
    // [{x, y, tx, ty}, {...}, {...}, {...}]
    static float s_textVertexBuffer[MAX_TEXT_CHARACTERS * 4 * 4] = {};
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

    static bool CPInsideDiv(TextContext & context)
    {
      GlyphData * pData = &s_glyphData[context.cpCurrentPosition].data;
      vec2 position(float(context.posX) + float(pData->bearingX + pData->width), 
                    float(context.lineY) + float(pData->height - pData->bearingY));
      vec2 size(float(pData->width), float(pData->height));
      UIAABB glyphBounds = {position, size};
      UIAABB result = {};
      return Intersection(context.divViewable, glyphBounds, result);
    }

    // Writes character and moves to the next.
    // Assumes will not overflow.
    static void WriteCharacter(TextContext & context)
    {
      GlyphData * pData = &s_glyphData[context.cpCurrentPosition].data;
      if (CPInsideDiv(context) && (pData->textureID == context.currentTextureID))
      {
        float y = float(context.lineY);
        float x = float(context.posX);

        for (int i = 0; i < 4; i++)
        {
          uint32_t ind = context.writtenCPs * 16;

          s_textVertexBuffer[ind + 0] = x + float(pData->bearingX + pData->width * (i & 1));
          s_textVertexBuffer[ind + 1] = y + float(-pData->bearingY + pData->height * (i >> 1));
          s_textVertexBuffer[ind + 2] = float(pData->posX + pData->width * (i & 1));
          s_textVertexBuffer[ind + 3] = float(pData->posY + pData->height * (i >> 1));
        }
        context.writtenCPs++;
      }

      context.posX += int32_t(pData->advance);
      context.cpCurrentPosition++;
    }

    static void WriteBlock(TextContext & context, uint32_t cpCount)
    {
      for (uint32_t i = 0; i < cpCount; i++)
        WriteCharacter(context);
    }

    static int32_t GetLength(uint32_t a_index, uint32_t a_count)
    {
      int32_t result = 0;
      if (a_count == 0)
        return result;

      for (uint32_t i = a_index; (i + 1) < a_index + a_count; i++)
        result = (int32_t) s_glyphData[i].data.advance;

      result += ((int32_t)s_glyphData[a_index + a_count - 1].data.bearingX + (int32_t)s_glyphData[a_index + a_count - 1].data.width);

      return result;
    }

    // Return word length. 0 if EOL or EOT reached
    static int FindNextWord(TextContext & context)
    {
      bool wordFound = false;

      for (; context.cpCurrentPosition < context.cpCount; context.cpCurrentPosition++)
      {
        CodePoint cp = s_glyphData[context.cpCurrentPosition].cp;

        if (IsWhiteSpace(cp))
          continue;

        if (IsNewLine(cp))
        {
          context.cpCurrentPosition++;
          break;
        }

        wordFound = true;
        break;
      }
      
      int cpCount = 0;
      if (wordFound)
      {
        for (; context.cpCurrentPosition < context.cpCount; context.cpCurrentPosition++)
        {
          CodePoint cp = s_glyphData[context.cpCurrentPosition + cpCount].cp;
          if (IsWhiteSpace(cp) || IsNewLine(cp))
            break;
          cpCount++;
        }
      }

      return cpCount;
    }

    // Returns true if more lines to process
    static bool WriteNextLine(TextContext & context)
    {
      int cpCount = FindNextWord(context);

      // Line contained only whitespace
      if (cpCount > 0)
      {
        int32_t wordLen = GetLength(context.cpCurrentPosition, cpCount);
        bool wordWritten = false;
        while ((context.posX + wordLen) <= context.div.size.x())
        {
          wordWritten = true;
          WriteBlock(context, cpCount);
          cpCount = FindNextWord(context);
          wordLen = GetLength(context.cpCurrentPosition, cpCount);
        }

        // We need at least 1 character on the line, regardless if it fits or not.
        if (!wordWritten)
          WriteCharacter(context);
      }

      return context.cpCurrentPosition == context.cpCount;
    }

    static void WriteText(TextContext & context)
    {
      context.cpCurrentPosition = 0;
      context.writtenCPs = 0;

      int32_t line = 0;
      int32_t posX = context.posX;
      int32_t lineY = context.lineY;

      while (WriteNextLine(context))
      {
        line++;
        context.posX = posX;
        context.lineY = lineY + line * context.lineSpacing;
        if (context.lineY > context.divViewable.size.y())
          break;
      }
    }

    static uint32_t DecodeText(std::string const & a_str, uint32_t & a_textureCount)
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
            GlyphData * pData = Renderer::GetGlyphData(cp);
            if (pData == nullptr)
              pData = Renderer::GetGlyphData(uint32_t('?'));
            BSR_ASSERT(pData != nullptr, "Default font atlas missing '?' character");
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

    Text::Text(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags)
      : Widget(flags)
      , m_pParent(pParent)
      , m_text(text)
      , m_attributes{}
      , m_aabb{position, size}
    {
      if (pAttrs != nullptr)
      {
        m_attributes = *pAttrs;
      }
      else
      {
        // TODO these need to come from settings
        m_attributes.size = DEFAULT_FONT_SIZE;
        m_attributes.colourText = 0xFFFFFFFF;
        m_attributes.horizontalAlign = TextAlignment::Min;
        m_attributes.verticalAlign = TextAlignment::Min;
        m_attributes.wrapText = true;
      }
    }

    Text * Text::Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, TextAttributes const * pAttrs, std::initializer_list<WidgetFlag> flags)
    {
      return new Text(pParent, text, position, size, pAttrs, flags);
    }

    Text::~Text()
    {

    }

    void Text::SetText(std::string const & a_str)
    {
      m_text = a_str;
    }

    void Text::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalAABB(viewableWindow))
        return;

      int16_t ascent, descent;
      uint32_t textureCount;

      Renderer::GetCharacterSizeRange(ascent, descent);

      TextContext context;
      context.div = {m_pParent->GetContentDivPosition(), m_pParent->GetContentDivSize()};

      if (!m_attributes.wrapText)
        context.div.size.y() = std::numeric_limits<float>::max();

      context.divViewable = viewableWindow;
      context.horizontalAlign = m_attributes.horizontalAlign;
      context.lineSpacing = int16_t(m_attributes.lineSpacing * (ascent - descent));
      context.cpCount = DecodeText(m_text, textureCount);
      
      ::Engine::Renderer::Enable(RenderFeature::Sissor);
      ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      
      for (uint32_t i = 0; i < textureCount; i++)
      {
        context.posX = int16_t(context.div.position.x());
        context.lineY = int16_t(context.div.position.y() + ascent);
        context.currentTextureID = s_textureIDs[i];

        WriteText(context);

        Renderer::DrawText(context.currentTextureID, m_attributes.colourText, context.writtenCPs, s_textVertexBuffer);
      }
      
      ::Engine::Renderer::Disable(RenderFeature::Sissor);
    }
    
    WidgetState Text::QueryState() const
    {
      return WidgetState::None;
    }

    Widget * Text::GetParent() const
    {
      return m_pParent;
    }

    void Text::SetParent(Widget * a_pParent)
    {
      m_pParent = a_pParent;
    }

    void Text::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      DISPATCH_MESSAGE(Message_GUI_PointerMove);
      DISPATCH_MESSAGE(Message_GUI_PointerDown);
    }

    void Text::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
    }

    void Text::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb))
        a_pMsg->ConsumeHover();
    }

    void Text::_SetLocalPosition(vec2 const & a_pos)
    {
      m_aabb.position = a_pos;
    }

    void Text::_SetSize(vec2 const & a_size)
    {
      m_aabb.size = a_size;
    }

    vec2 Text::_GetLocalPosition()
    {
      return m_aabb.position;
    }

    vec2 Text::_GetSize()
    {
      return m_aabb.size;
    }
  }
}