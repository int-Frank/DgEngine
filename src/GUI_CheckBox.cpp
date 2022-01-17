//@group GUI

#include "GUI_Checkbox.h"
#include "MessageHandler.h"
#include "GUI_Internal.h"
#include "GUI.h"
#include "Renderer.h"

#define CHECKBOX_SIZE vec2(18.0f, 18.0f)
#define CHECKBOX_THICKNESS 2.0f

namespace DgE
{
  namespace GUI
  {
    class Checkbox::PIMPL
    {
    public:

      PIMPL()
        : isChecked(false)
        , style{}
        , aabb{}
        , state(WidgetState::None)
        , pParent(nullptr)
        , clbk_CheckChanged(nullptr)
        , clbk_HoverOn(nullptr)
        , clbk_HoverOff(nullptr)
      {}

      void HandleMessage(Message_GUI_PointerDown *, Checkbox *pCheckbox);
      void HandleMessage(Message_GUI_PointerMove *, Checkbox *pCheckbox);

      bool isChecked;
      Style::Checkbox style;
      UIAABB aabb;
      WidgetState state;
      Widget *pParent;

      std::function<void(bool)> clbk_CheckChanged;
      std::function<void()> clbk_HoverOn;
      std::function<void()> clbk_HoverOff;
    };

    Checkbox::Checkbox(vec2 const & a_position, bool a_checked, Style::Checkbox const &style, std::initializer_list<WidgetFlag> flags)
      : Widget({WidgetFlag::NotResponsive}, flags)
      , m_pimpl(new PIMPL())
    {
      SetStyle(style);
      m_pimpl->aabb = {a_position, CHECKBOX_SIZE};
    }

    Checkbox::~Checkbox()
    {
      delete m_pimpl;
    }

    Checkbox *Checkbox::Create(vec2 const &position, bool a_checked, std::initializer_list<WidgetFlag> flags)
    {
      return new Checkbox(position, a_checked, s_style, flags);
    }

    Checkbox *Checkbox::Create(vec2 const &position, bool a_checked, Style::Checkbox const &style, std::initializer_list<WidgetFlag> flags)
    {
      return new Checkbox(position, a_checked, style, flags);
    }

    Style::Checkbox const &Checkbox::GetStyle() const
    {
      return m_pimpl->style;
    }

    void Checkbox::SetStyle(Style::Checkbox const &style)
    {
      m_pimpl->style = style;
    }

    void Checkbox::BindCheckedChanged(std::function<void(bool)> a_fn)
    {
      m_pimpl->clbk_CheckChanged = a_fn;
    }

    void Checkbox::BindHoverOn(std::function<void()> a_fn)
    {
      m_pimpl->clbk_HoverOn = a_fn;
    }

    void Checkbox::BindHoverOff(std::function<void()> a_fn)
    {
      m_pimpl->clbk_HoverOff = a_fn;
    }

    void Checkbox::_HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return;

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg), this);
      else if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(a_pMsg), this);
    }

    void Checkbox::PIMPL::HandleMessage(Message_GUI_PointerDown * a_pMsg, Checkbox *pCheckbox)
    {
      UIAABB aabb;
      if (!pCheckbox->GetGlobalViewableArea(aabb))
        return;

      if (PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), aabb) && clbk_CheckChanged != nullptr)
      {
        isChecked = !isChecked;
        clbk_CheckChanged(isChecked);
        a_pMsg->SetFlag(DgE::Message::Flag::Handled, true);
      }
    }

    void Checkbox::PIMPL::HandleMessage(Message_GUI_PointerMove * a_pMsg, Checkbox *pCheckbox)
    {
      UIAABB aabb;
      if (!pCheckbox->GetGlobalViewableArea(aabb))
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

    void Checkbox::ClearBindings()
    {
      m_pimpl->clbk_CheckChanged = nullptr;
    }

    void Checkbox::Draw()
    {
      UIAABB viewableWindow;
      if (!GetGlobalViewableArea(viewableWindow))
        return;

      vec2 size = GetSize() - 2.0f * vec2(CHECKBOX_THICKNESS, CHECKBOX_THICKNESS);

      vec2 pos = GetGlobalPosition() + vec2(CHECKBOX_THICKNESS, CHECKBOX_THICKNESS);
      int s = m_pimpl->state == WidgetState::HoverOn ? (int)Style::Button::Hover : (int)Style::Button::Default;

      ::DgE::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
      Renderer::DrawBoxWithBorder({pos, size}, CHECKBOX_THICKNESS, m_pimpl->style.colours[s].box, m_pimpl->style.colours[s].border);

      if (m_pimpl->isChecked)
      {
        vec2 grabPos = pos + vec2(1.0f, 1.0f);
        Renderer::DrawGlyph(10004, GUI_FONT_SIZE_TICK, grabPos, m_pimpl->style.tick);
      }
    }

    WidgetState Checkbox::QueryState() const
    {
      return m_pimpl->state;
    }

    Widget * Checkbox::GetParent() const
    {
      return m_pimpl->pParent;
    }

    void Checkbox::SetParent(Widget * a_pParent)
    {
      m_pimpl->pParent = a_pParent;
    }

    vec2 Checkbox::GetLocalPosition()
    {
      return m_pimpl->aabb.position;
    }

    vec2 Checkbox::GetSize()
    {
      return m_pimpl->aabb.size;
    }

    void Checkbox::SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->aabb.position = a_pos;
    }

    void Checkbox::SetSize(vec2 const & a_size)
    {
      m_pimpl->aabb.size = a_size;
    }

    void Checkbox::SetChecked(bool a_val)
    {
      if (a_val == m_pimpl->isChecked)
        return;

      m_pimpl->isChecked = a_val;
      if (m_pimpl->clbk_CheckChanged != nullptr)
        m_pimpl->clbk_CheckChanged(m_pimpl->isChecked);
    }
  }
}