//@group GUI

#include "GUI_Checkbox.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "GUI.h"
#include "GUI_Text.h"
#include "Renderer.h"

#define CHECKBOX_SIZE vec2(25.0f, 25.0f)

namespace Engine
{
  namespace GUI
  {
    CheckBox::CheckBox(Widget * a_pParent, vec2 const & a_position, std::initializer_list<WidgetFlag> flags)
      : Widget(flags)
      , m_isSelected(false)
      , m_pTextTick(nullptr)
      , m_clrBox(GetStyle().colours[col_Checkbox])
      , m_clrTick(GetStyle().colours[col_CheckboxTick])
      , m_aabb{a_position, CHECKBOX_SIZE}
      , m_pParent(a_pParent)
      , m_state(WidgetState::None)
      , m_clbk_ChangeSelected(nullptr)
    {
      TextAttributes attr = {};
      attr.size = GUI_FONT_SIZE_TICK;
      attr.colourText = m_clrTick;
      attr.horizontalAlign = HorizontalAlignment::Centre;
      attr.verticalAlign = VerticalAlignment::Centre;
      attr.lineSpacing = GetStyle().textLineSpacing;
      attr.wrapText = false;

      m_pTextTick = Text::Create(this, "\xE2\x9C\x94", {0.f, 0.f}, CHECKBOX_SIZE, &attr,
        {WidgetFlag::NotResponsive, WidgetFlag::StretchHeight, WidgetFlag::StretchWidth});
    }

    CheckBox::~CheckBox()
    {
      delete m_pTextTick;
    }

    CheckBox * CheckBox::Create(Widget * pParent, vec2 const & position, std::initializer_list<WidgetFlag> flags)
    {
      return new CheckBox(pParent, position, flags);
    }

    void CheckBox::BindChangeSelected(std::function<void(bool)> a_fn)
    {
      m_clbk_ChangeSelected = a_fn;
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

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && m_clbk_ChangeSelected != nullptr)
      {
        m_isSelected = !m_isSelected;
        m_clbk_ChangeSelected(m_isSelected);
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
        m_state = WidgetState::HoverOn;
      if (!isInside && m_state == WidgetState::HoverOn)
        m_state = WidgetState::None;
    }

    void CheckBox::ClearBindings()
    {
      m_clbk_ChangeSelected = nullptr;
    }

    void CheckBox::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalAABB(viewableWindow))
        return;

      ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());

      float th = GetStyle().checkboxThickness;
      vec2 pos = GetGlobalPosition() + vec2(th, th);
      vec2 size = GetSize() - 2.0f * vec2(th, th);
      Renderer::DrawBoxBorder({pos, size}, th, m_clrBox);

      if (m_isSelected)
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
  }
}