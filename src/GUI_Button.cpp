//@group GUI

#include "GUI_Button.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "GUI_Text.h"
#include "Renderer.h"
#include "unicode.h"
#include "Log.h"

namespace DgE
{
  namespace GUI
  {
    class Button::PIMPL
    {
    public:

      PIMPL()
        : pText(nullptr)
        , codePoint(INVALID_CHAR)
        , aabb{}
        , state(WidgetState::None)
        , pParent(nullptr)
        , style{}
        , clbk_HoverOn(nullptr)
        , clbk_HoverOff(nullptr)
        , clbk_Select(nullptr)
      {}

      ~PIMPL() { delete pText; }

      void HandleMessage(Message_GUI_PointerDown *, Button *pButton);
      void HandleMessage(Message_GUI_PointerMove *, Button *pButton);

      Text *pText;
      CodePoint codePoint;
      UIAABB aabb;
      WidgetState state;
      Widget *pParent;

      Style::Button style;

      std::function<void()> clbk_HoverOn;
      std::function<void()> clbk_HoverOff;
      std::function<void()> clbk_Select;
    };


    void Button::PIMPL::HandleMessage(Message_GUI_PointerDown *a_pMsg, Button *pButton)
    {
      UIAABB aabb;
      if (!pButton->GetGlobalViewableArea(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && clbk_Select != nullptr)
      {
        clbk_Select();
        a_pMsg->SetFlag(DgE::Message::Flag::Handled, true);
      }
    }

    void Button::PIMPL::HandleMessage(Message_GUI_PointerMove *a_pMsg, Button *pButton)
    {
      UIAABB aabb;
      if (!pButton->GetGlobalViewableArea(aabb))
        return;

      bool isInside = PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb);
      if (isInside)
        a_pMsg->ConsumeHover();

      if (isInside && state == WidgetState::None)
      {
        state = WidgetState::HoverOn;
        if (clbk_HoverOn != nullptr)
          clbk_HoverOn();
      }
      if (!isInside && state == WidgetState::HoverOn)
      {
        state = WidgetState::None;
        if (clbk_HoverOff != nullptr)
          clbk_HoverOff();
      }
    }


    Style::Button const & Button::GetDefaultStyle()
    {
      return s_style;
    }

    Button::Button(std::string const & a_text, bool isGlyph, vec2 const & a_position, vec2 const & a_size, Style::Button const &style, std::initializer_list<WidgetFlag> flags)
      : Widget({WidgetFlag::NotResponsive,
                WidgetFlag::StretchWidth,
                WidgetFlag::StretchHeight}, flags)
      , m_pimpl(new PIMPL())
    {
      SetStyle(style);
      m_pimpl->aabb = {a_position, a_size};

      if (isGlyph)
      {
        UTF8Parser p(a_text.c_str());
        m_pimpl->codePoint = p.Next();
        if (m_pimpl->codePoint == INVALID_CHAR)
          LOG_WARN("Cound not interpret utf8 text when creating button: %s", a_text.c_str());
      }
      else
      {
        m_pimpl->pText = Text::Create(a_text, {0.f, 0.f}, a_size, m_pimpl->style.text[Style::Button::Default],
          {WidgetFlag::NotResponsive, WidgetFlag::StretchHeight, WidgetFlag::StretchWidth});
        m_pimpl->pText->SetParent(this);
      }
    }

    Button::~Button()
    {
      delete m_pimpl;
    }

    Button *Button::Create(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags)
    {
      return new Button(text, false, position, size, s_style, flags);
    }

    Button *Button::Create(std::string const &text, vec2 const &position, vec2 const &size, Style::Button const &style, std::initializer_list<WidgetFlag> flags)
    {
      return new Button(text, false, position, size, style, flags);
    }

    Button *Button::CreateWithGlyph(std::string const &text, vec2 const &position, vec2 const &size, std::initializer_list<WidgetFlag> flags)
    {
      return new Button(text, true, position, size, s_style, flags);
    }

    Button *Button::CreateWithGlyph(std::string const &text, vec2 const &position, vec2 const &size, Style::Button const &style, std::initializer_list<WidgetFlag> flags)
    {
      return new Button(text, true, position, size, style, flags);
    }

    Style::Button const &Button::GetStyle() const
    {
      return m_pimpl->style;
    }

    void Button::SetStyle(Style::Button const & style)
    {
      m_pimpl->style = style;
      m_pimpl->style.text[Style::Button::Default].size = GUI_FONT_SIZE;
      m_pimpl->style.text[Style::Button::Hover].size = GUI_FONT_SIZE;
    }

    void Button::BindHoverOn(std::function<void()> a_fn)
    {
      m_pimpl->clbk_HoverOn = a_fn;
    }

    void Button::BindHoverOff(std::function<void()> a_fn)
    {
      m_pimpl->clbk_HoverOff = a_fn;
    }

    void Button::BindSelect(std::function<void()> a_fn)
    {
      m_pimpl->clbk_Select = a_fn;
    }

    void Button::_HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg), this);
      else if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(a_pMsg), this);
    }

    void Button::ClearBindings()
    {
      m_pimpl->clbk_HoverOn = nullptr;
      m_pimpl->clbk_HoverOff = nullptr;
      m_pimpl->clbk_Select = nullptr;
    }

    void Button::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalViewableArea(viewableWindow))
        return;

      vec2 size = GetSize() - 2.0f * vec2(m_pimpl->style.borderWidth, m_pimpl->style.borderWidth);
      if (size.x() < 0.0f)
        size.x() = 0.0f;
      if (size.y() < 0.0f)
        size.y() = 0.0f;

      vec2 pos = GetGlobalPosition() + vec2(m_pimpl->style.borderWidth, m_pimpl->style.borderWidth);
      int s = m_pimpl->state == WidgetState::HoverOn ? (int)Style::Button::Hover : (int)Style::Button::Default;

      ::DgE::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      
      if (m_pimpl->style.borderWidth == 0.0f)
        Renderer::DrawBox({pos, size}, m_pimpl->style.colours[s].face);
      else
        Renderer::DrawBoxWithBorder({pos, size}, m_pimpl->style.borderWidth, m_pimpl->style.colours[s].face, m_pimpl->style.colours[s].border);

      if (m_pimpl->pText != nullptr)
      {
        m_pimpl->pText->SetStyle(m_pimpl->style.text[s]);
        m_pimpl->pText->Draw();
      }
      else if (m_pimpl->codePoint != INVALID_CHAR)
      {
        vec2 grabPos = pos + vec2(m_pimpl->style.contentMargin, m_pimpl->style.contentMargin);
        Renderer::DrawGlyph(m_pimpl->codePoint, GUI_FONT_SIZE, grabPos, m_pimpl->style.text[s].colourText);
      }
    }

    WidgetState Button::QueryState() const
    {
      return m_pimpl->state;
    }

    Widget * Button::GetParent() const
    {
      return m_pimpl->pParent;
    }

    void Button::SetParent(Widget * a_pParent)
    {
      m_pimpl->pParent = a_pParent;
    }

    vec2 Button::_GetLocalPosition()
    {
      return m_pimpl->aabb.position;
    }

    vec2 Button::_GetSize()
    {
      return m_pimpl->aabb.size;
    }

    void Button::_SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->aabb.position = a_pos;
    }

    void Button::_SetSize(vec2 const & a_size)
    {
      m_pimpl->aabb.size = a_size;
    }

    void Button::SetText(std::string const & a_str)
    {
      m_pimpl->pText->SetText(a_str);
    }
  }
}