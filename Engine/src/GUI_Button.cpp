//@group GUI

#include "GUI_Button.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "GUI.h"
#include "GUI_Text.h"
#include "Renderer.h"

namespace Engine
{
  namespace GUI
  {

    Button::Button(Widget * a_pParent, std::string const & a_text, vec2 const & a_position, vec2 const & a_size, std::initializer_list<WidgetFlag> flags)
      : Widget({WidgetFlag::NotResponsive,
                WidgetFlag::StretchWidth,
                WidgetFlag::StretchHeight}, flags)
      , m_pText(nullptr)
      , m_aabb{a_position, a_size}
      , m_state(WidgetState::None)
      , m_pParent(a_pParent)
      , m_clr{}
      , m_contentMargin(0.0f)
      , m_outlineWidth(0.0f)
      , m_clbk_HoverOn(nullptr)
      , m_clbk_HoverOff(nullptr)
      , m_clbk_Select(nullptr)
    {
      TextAttributes attr = {};
      attr.size = GUI_FONT_SIZE;
      attr.colourText = GetStyle().colours[col_ButtonText];
      attr.horizontalAlign = HorizontalAlignment::Centre;
      attr.verticalAlign = VerticalAlignment::Centre;
      attr.lineSpacing = GetStyle().textLineSpacing;
      attr.wrapText = true;

      m_clr[(int)ButtonState::Normal][(int)ButtonElement::Face] = GetStyle().colours[col_ButtonFace];
      m_clr[(int)ButtonState::Normal][(int)ButtonElement::Outline] = GetStyle().colours[col_ButtonOutline];
      m_clr[(int)ButtonState::Normal][(int)ButtonElement::Text] = GetStyle().colours[col_ButtonText];
      m_clr[(int)ButtonState::Hover][(int)ButtonElement::Face] = GetStyle().colours[col_ButtonFaceHover];
      m_clr[(int)ButtonState::Hover][(int)ButtonElement::Outline] = GetStyle().colours[col_ButtonOutlineHover];
      m_clr[(int)ButtonState::Hover][(int)ButtonElement::Text] = GetStyle().colours[col_ButtonTextHover];

      m_contentMargin = GetStyle().contentMargin;
      m_outlineWidth = GetStyle().outlineWidth;

      m_pText = Text::Create(this, a_text, {0.f, 0.f}, a_size, &attr, 
        {WidgetFlag::NotResponsive, WidgetFlag::StretchHeight, WidgetFlag::StretchWidth});
    }

    Button::~Button()
    {
      delete m_pText;
    }

    Button * Button::Create(Widget * pParent, std::string const & text, vec2 const & position, vec2 const & size, std::initializer_list<WidgetFlag> flags)
    {
      return new Button(pParent, text, position, size, flags);
    }

    void Button::BindHoverOn(std::function<void()> a_fn)
    {
      m_clbk_HoverOn = a_fn;
    }

    void Button::BindHoverOff(std::function<void()> a_fn)
    {
      m_clbk_HoverOff = a_fn;
    }

    void Button::BindSelect(std::function<void()> a_fn)
    {
      m_clbk_Select = a_fn;
    }

    void Button::_HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      DISPATCH_MESSAGE(Message_GUI_PointerMove);
      DISPATCH_MESSAGE(Message_GUI_PointerDown);
    }

    void Button::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && m_clbk_Select != nullptr)
      {
        m_clbk_Select();
        a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
      }
    }

    void Button::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      bool isInside = PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb);
      if (isInside)
        a_pMsg->ConsumeHover();

      if (isInside && m_state == WidgetState::None)
      {
        m_state = WidgetState::HoverOn;
        if (m_clbk_HoverOn != nullptr)
          m_clbk_HoverOn();
      }
      if (!isInside && m_state == WidgetState::HoverOn)
      {
        m_state = WidgetState::None;
        if (m_clbk_HoverOff != nullptr)
          m_clbk_HoverOff();
      }
    }

    void Button::ClearBindings()
    {
      m_clbk_HoverOn = nullptr;
      m_clbk_HoverOff = nullptr;
      m_clbk_Select = nullptr;
    }

    void Button::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalAABB(viewableWindow))
        return;

      vec2 size = GetSize() - 2.0f * vec2(m_outlineWidth, m_outlineWidth);
      if (size.x() < 0.0f)
        size.x() = 0.0f;
      if (size.y() < 0.0f)
        size.y() = 0.0f;

      vec2 pos = GetGlobalPosition() + vec2(m_outlineWidth, m_outlineWidth);
      int s = m_state == WidgetState::HoverOn ? (int)ButtonState::Hover : (int)ButtonState::Normal;

      ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      Renderer::DrawBoxWithOutline({pos, size}, m_outlineWidth, m_clr[s][(int)ButtonElement::Face], m_clr[s][(int)ButtonElement::Outline]);

      m_pText->SetColour(m_clr[s][(int)ButtonElement::Text]);
      m_pText->Draw();
    }

    WidgetState Button::QueryState() const
    {
      return m_state;
    }

    Widget * Button::GetParent() const
    {
      return m_pParent;
    }

    void Button::SetParent(Widget * a_pParent)
    {
      m_pParent = a_pParent;
    }

    vec2 Button::_GetLocalPosition()
    {
      return m_aabb.position;
    }

    vec2 Button::_GetSize()
    {
      return m_aabb.size;
    }

    void Button::_SetLocalPosition(vec2 const & a_pos)
    {
      m_aabb.position = a_pos;
    }

    void Button::_SetSize(vec2 const & a_size)
    {
      m_aabb.size = a_size;
    }

    vec2 Button::GetContentDivPosition()
    {
      return vec2(m_contentMargin, m_contentMargin);
    }

    vec2 Button::GetContentDivSize()
    {
      vec2 size = GetSize() - vec2(m_contentMargin * 2.0f, m_contentMargin * 2.0f);

      if ((size[0] <= 0.0f) || (size[1] <= 0.0f))
        size.Zero();

      return size;
    }

    void Button::SetColour(ButtonState a_state, ButtonElement a_ele, Colour a_clr)
    {
      BSR_ASSERT(a_state != ButtonState::COUNT);
      BSR_ASSERT(a_ele != ButtonElement::COUNT);
      m_clr[(int)a_state][(int)a_ele] = a_clr;
    }

    void Button::SetContentMargin(float a_val)
    {
      m_contentMargin = a_val;
    }
  }
}