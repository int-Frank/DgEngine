//@group GUI

#include "GUI_Checkbox.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "GUI.h"
#include "GUI_Text.h"
#include "Renderer.h"

#define CHECKBOX_SIZE vec2(25.0f, 25.0f)
#define CHECKBOX_THICKNESS 2.0f

namespace Engine
{
  namespace GUI
  {
    CheckBox::CheckBox(Widget * a_pParent, vec2 const & a_position, bool a_checked, std::initializer_list<WidgetFlag> flags)
      : Widget({WidgetFlag::NotResponsive}, flags)
      , m_isChecked(a_checked)
      , m_pTextTick(nullptr)
      , m_clr{}
      , m_aabb{a_position, CHECKBOX_SIZE}
      , m_state(WidgetState::None)
      , m_pParent(a_pParent)
      , m_clbk_CheckChanged(nullptr)
    {
      TextAttributes attr = {};
      attr.size = GUI_FONT_SIZE_TICK;
      attr.colourText = GetStyle().colours[col_CheckboxTick];
      attr.horizontalAlign = HorizontalAlignment::Centre;
      attr.verticalAlign = VerticalAlignment::Centre;
      attr.lineSpacing = GetStyle().textLineSpacing;
      attr.wrapText = false;

      m_pTextTick = Text::Create(this, "\xE2\x9C\x94", {0.f, 0.f}, CHECKBOX_SIZE, &attr,
        {WidgetFlag::NotResponsive, WidgetFlag::StretchHeight, WidgetFlag::StretchWidth});

      m_clr[(int)CheckboxState::Normal][(int)CheckboxElement::Outline] = GetStyle().colours[col_Checkbox];
      m_clr[(int)CheckboxState::Normal][(int)CheckboxElement::Tick] = GetStyle().colours[col_CheckboxTick];
      m_clr[(int)CheckboxState::Hover][(int)CheckboxElement::Outline] = GetStyle().colours[col_CheckboxHover];
      m_clr[(int)CheckboxState::Hover][(int)CheckboxElement::Tick] = GetStyle().colours[col_CheckboxTickHover];
    }

    CheckBox::~CheckBox()
    {
      delete m_pTextTick;
    }

    CheckBox * CheckBox::Create(Widget * pParent, vec2 const & position, bool a_checked, std::initializer_list<WidgetFlag> flags)
    {
      return new CheckBox(pParent, position, a_checked, flags);
    }

    void CheckBox::BindCheckedChanged(std::function<void(bool)> a_fn)
    {
      m_clbk_CheckChanged = a_fn;
    }

    void CheckBox::BindHoverOn(std::function<void()> a_fn)
    {
      m_clbk_HoverOn = a_fn;
    }

    void CheckBox::BindHoverOff(std::function<void()> a_fn)
    {
      m_clbk_HoverOff = a_fn;
    }

    void CheckBox::_HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      DISPATCH_MESSAGE(Message_GUI_PointerMove);
      DISPATCH_MESSAGE(Message_GUI_PointerDown);
    }

    void CheckBox::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!GetGlobalAABB(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && m_clbk_CheckChanged != nullptr)
      {
        m_isChecked = !m_isChecked;
        m_clbk_CheckChanged(m_isChecked);
        a_pMsg->SetFlag(Engine::Message::Flag::Handled, true);
      }
    }

    void CheckBox::HandleMessage(Message_GUI_PointerMove * a_pMsg)
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

    void CheckBox::ClearBindings()
    {
      m_clbk_CheckChanged = nullptr;
    }

    void CheckBox::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalAABB(viewableWindow))
        return;

      ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());

      vec2 pos = GetGlobalPosition() + vec2(CHECKBOX_THICKNESS, CHECKBOX_THICKNESS);
      vec2 size = GetSize() - 2.0f * vec2(CHECKBOX_THICKNESS, CHECKBOX_THICKNESS);

      int s = m_state == WidgetState::HoverOn ? (int)CheckboxState::Hover : (int)CheckboxState::Normal;
      Renderer::DrawBoxOutline({pos, size}, CHECKBOX_THICKNESS, m_clr[s][(int)CheckboxElement::Outline]);

      if (m_isChecked)
        m_pTextTick->Draw();
    }

    WidgetState CheckBox::QueryState() const
    {
      return m_state;
    }

    Widget * CheckBox::GetParent() const
    {
      return m_pParent;
    }

    void CheckBox::SetParent(Widget * a_pParent)
    {
      m_pParent = a_pParent;
    }

    vec2 CheckBox::_GetLocalPosition()
    {
      return m_aabb.position;
    }

    vec2 CheckBox::_GetSize()
    {
      return m_aabb.size;
    }

    void CheckBox::_SetLocalPosition(vec2 const & a_pos)
    {
      m_aabb.position = a_pos;
    }

    void CheckBox::_SetSize(vec2 const & a_size)
    {
      m_aabb.size = a_size;
    }

    vec2 CheckBox::GetContentDivPosition()
    {
      return vec2(0.0f, 0.0f);
    }

    vec2 CheckBox::GetContentDivSize()
    {
      return CHECKBOX_SIZE;
    }

    void CheckBox::SetColour(CheckboxState a_state, CheckboxElement a_ele, Colour a_clr)
    {
      BSR_ASSERT(a_state != CheckboxState::COUNT);
      BSR_ASSERT(a_ele != CheckboxElement::COUNT);
      m_clr[(int)a_state][(int)a_ele] = a_clr;
    }
    
    void CheckBox::SetChecked(bool a_val)
    {
      if (a_val == m_isChecked)
        return;

      m_isChecked = a_val;
      if (m_clbk_CheckChanged != nullptr)
        m_clbk_CheckChanged(m_isChecked);
    }
  }
}