//@group GUI

#include "Utils.h"
#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Slider.h"
#include "Renderer.h"

#define SLIDER_MIN_RUN_PIXELS 4.0f
#define SLIDER_MIN_CARET_WIDTH 2.0f

#define SLIDER_HEIGHT m_pContext->

namespace DgE
{
  namespace GUI
  {
    class SliderContext
    {
    public:

      SliderContext()
        : value(0)
        , length(100.0f)
        , isVertical(false)
        , position{}
        , style{}
        , pSlider{}
        , pParent{}
        , clbk_HoverOn()
        , clbk_HoverOff()
      {
        if (clbk_HoverOn == nullptr)
          char t = 0;
      }

      float value; // normalised between 0 and 1
      float length;
      bool isVertical;
      vec2 position;
      Style::Slider style;
      SliderBase *pSlider;
      Widget *pParent;

      std::function<void()> clbk_HoverOn;
      std::function<void()> clbk_HoverOff;

      float GetLowerLength();
      void GetAABBs(UIAABB &lower, UIAABB &upper, UIAABB &caret);
      void GetInnerAABBs(UIAABB &lower, UIAABB &upper, UIAABB &caret);
      void SetValFromScreenCoord(vec2 const & point);
    };

    void SliderContext::GetAABBs(UIAABB &a_lower, UIAABB &a_upper, UIAABB &a_caret)
    {
      vec2 globalPos = pSlider->GetGlobalPosition();
      vec2 size = pSlider->GetSize();

      if (!isVertical)
      {
        a_lower.position.x() = globalPos.x();
        a_lower.position.y() = globalPos.y() + (size.y() - style.components[Style::Slider::c_LowerBar].height) / 2.0f;
        a_lower.size.x() = GetLowerLength();
        a_lower.size.y() = style.components[Style::Slider::c_LowerBar].height;

        a_upper.position.x() = globalPos.x() + GetLowerLength();
        a_upper.position.y() = globalPos.y() + (size.y() - style.components[Style::Slider::c_UpperBar].height) / 2.0f;
        a_upper.size.x() = length - GetLowerLength();
        a_upper.size.y() = style.components[Style::Slider::c_UpperBar].height;

        a_caret.position.x() = globalPos.x() + GetLowerLength() - style.caretWidth / 2.0f;
        a_caret.position.y() = globalPos.y() + (size.y() - style.components[Style::Slider::c_Caret].height) / 2.0f;
        a_caret.size.x() = style.caretWidth;
        a_caret.size.y() = style.components[Style::Slider::c_Caret].height;
      }
      else
      {
        a_lower.position.x() = globalPos.x() + (size.x() - style.components[Style::Slider::c_LowerBar].height) / 2.0f;
        a_lower.position.y() = globalPos.y() + length - GetLowerLength();
        a_lower.size.x() = style.components[Style::Slider::c_LowerBar].height;
        a_lower.size.y() = GetLowerLength();

        a_upper.position.x() = globalPos.x() + (size.x() - style.components[Style::Slider::c_UpperBar].height) / 2.0f;
        a_upper.position.y() = globalPos.y();
        a_upper.size.x() = style.components[Style::Slider::c_UpperBar].height;
        a_upper.size.y() = length - GetLowerLength();

        a_caret.position.x() = globalPos.x() + (size.x() - style.components[Style::Slider::c_Caret].height) / 2.0f;
        a_caret.position.y() = globalPos.y() + length - GetLowerLength() - style.caretWidth / 2.0f;
        a_caret.size.x() = style.components[Style::Slider::c_Caret].height;
        a_caret.size.y() = style.caretWidth;
      }
    }

    void SliderContext::GetInnerAABBs(UIAABB &a_lower, UIAABB &a_upper, UIAABB &a_caret)
    {
      GetAABBs(a_lower, a_upper, a_caret);

      vec2 posOffsetLower(style.components[Style::Slider::c_LowerBar].borderWidth, style.components[Style::Slider::c_LowerBar].borderWidth);
      vec2 posOffsetUpper(style.components[Style::Slider::c_UpperBar].borderWidth, style.components[Style::Slider::c_UpperBar].borderWidth);
      vec2 posOffsetCaret(style.components[Style::Slider::c_Caret].borderWidth, style.components[Style::Slider::c_Caret].borderWidth);

      a_lower.position += posOffsetLower;
      a_upper.position += posOffsetUpper;
      a_caret.position += posOffsetCaret;

      a_lower.size -= 2.0f * posOffsetLower;
      a_upper.size -= 2.0f * posOffsetUpper;
      a_caret.size -= 2.0f * posOffsetCaret;
    }

    void SliderContext::SetValFromScreenCoord(vec2 const &point)
    {
      if (!isVertical)
      {
        float start = pSlider->GetGlobalPosition().x() + style.caretWidth / 2.0f;
        value = (point.x() - start) / (length - style.caretWidth);
      }
      else
      {
        float start = pSlider->GetGlobalPosition().y() + length - style.caretWidth / 2.0f;
        value = (start - point.y()) / (length - style.caretWidth);
      }

      if (value < 0.0f)
        value = 0.0f;
      else if (value > 1.0f)
        value = 1.0f;
    }

    float SliderContext::GetLowerLength()
    {
      return style.caretWidth / 2.0f + ((length - style.caretWidth) * value);
    }

    //------------------------------------------------------------------------------------
    // State class declarations
    //------------------------------------------------------------------------------------

    class SliderState
    {
    public:

      SliderState(SliderContext * a_pData);
      virtual ~SliderState() {};

      virtual WidgetState QueryState() const = 0;
      virtual SliderState * HandleMessage(Message *) = 0;

    protected:

      SliderContext * m_pContext;
    };

    class SliderStaticState : public SliderState
    {
    public:

      SliderStaticState(SliderContext * a_pData, WidgetState);
      ~SliderStaticState() {}

      WidgetState QueryState() const override;

      SliderState * HandleMessage(Message * a_pMsg) override;
      SliderState * HandleMessage(Message_GUI_PointerDown * a_pMsg);
      SliderState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      WidgetState m_state;
    };

    class SliderActiveState : public SliderState
    {
    public:

      SliderActiveState(SliderContext * a_pData, float caretOffset);
      ~SliderActiveState() {}

      WidgetState QueryState() const override;

      SliderState * HandleMessage(Message * a_pMsg) override;
      SliderState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      float m_offset;
    };

    //------------------------------------------------------------------------------------
    // SliderState
    //------------------------------------------------------------------------------------

    SliderState::SliderState(SliderContext * a_pData)
      : m_pContext(a_pData)
    {

    }

    //------------------------------------------------------------------------------------
    // SliderStaticState
    //------------------------------------------------------------------------------------

    SliderStaticState::SliderStaticState(SliderContext * a_pData, WidgetState a_state)
      : SliderState(a_pData)
      , m_state(a_state)
    {

    }

    WidgetState SliderStaticState::QueryState() const
    {
      return m_state;
    }

    SliderState * SliderStaticState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      SliderState * pResult = nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
        pResult = HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(a_pMsg));
      else if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        pResult = HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));
      return pResult;
    }

    SliderState * SliderStaticState::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!m_pContext->pSlider->GetGlobalViewableArea(aabb))
        return nullptr;

      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (!PointInBox(point, aabb))
        return nullptr;

      UIAABB barLower = {};
      UIAABB barUpper = {};
      UIAABB caret = {};
      m_pContext->GetAABBs(barLower, barUpper, caret);


      if (PointInBox(point, caret))
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);

        float caretOffset;
        if (!m_pContext->isVertical)
          caretOffset = aabb.position.x() + m_pContext->GetLowerLength() - point.x();
        else
          caretOffset = aabb.position.y() + m_pContext->length - m_pContext->GetLowerLength() - point.y();

        return new SliderActiveState(m_pContext, caretOffset);
      }
      else if (PointInBox(point, barLower) || PointInBox(point, barUpper))
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        m_pContext->SetValFromScreenCoord(point);
        m_pContext->pSlider->NewValueClbk(m_pContext->value);
        return new SliderActiveState(m_pContext, 0.0f);
      }

      return nullptr;
    }

    SliderState * SliderStaticState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      UIAABB barLower = {};
      UIAABB barUpper = {};
      UIAABB caret = {};
      m_pContext->GetAABBs(barLower, barUpper, caret);

      bool isInside = PointInBox(vec2((float)a_pMsg->x, (float)a_pMsg->y), caret);
      if (isInside)
        a_pMsg->ConsumeHover();

      if (isInside && (m_state == WidgetState::None))
      {
        m_state = WidgetState::HoverOn;
        if (m_pContext->clbk_HoverOn != nullptr)
          m_pContext->clbk_HoverOn();
      }
      if (!isInside && (m_state == WidgetState::HoverOn))
      {
        m_state = WidgetState::None;
        if (m_pContext->clbk_HoverOff != nullptr)
          m_pContext->clbk_HoverOff();
      }
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // SliderActiveState
    //------------------------------------------------------------------------------------

    SliderActiveState::SliderActiveState(SliderContext * a_pData, float a_offset)
      : SliderState(a_pData)
      , m_offset(a_offset)
    {

    }

    WidgetState SliderActiveState::QueryState() const
    {
      return WidgetState::HasFocus;
    }

    SliderState * SliderActiveState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new SliderStaticState(m_pContext, WidgetState::HoverOn);
      }

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

      return nullptr;
    }

    SliderState * SliderActiveState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);
      m_pContext->SetValFromScreenCoord(point + vec2(m_offset, m_offset));
      m_pContext->pSlider->NewValueClbk(m_pContext->value);
      a_pMsg->SetFlag(Message::Flag::Handled, true);
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // SliderBase
    //------------------------------------------------------------------------------------

    class SliderBase::PIMPL
    {
    public:

      PIMPL() : pState(nullptr), context() {}
      ~PIMPL() { delete pState; }

      SliderState *pState;
      SliderContext context;
    };

    SliderBase::SliderBase(vec2 const & a_position, float length, float a_value, Style::Slider const &style, bool isVertical, std::initializer_list<WidgetFlag> a_flags)
      : Widget({WidgetFlag::NotResponsive,
                WidgetFlag::StretchWidth
        }, a_flags)
      , m_pimpl(new PIMPL())
    {
      SetStyle(style);

      m_pimpl->context.isVertical = isVertical;

      if (m_pimpl->context.style.caretWidth < SLIDER_MIN_CARET_WIDTH)
        m_pimpl->context.style.caretWidth = SLIDER_MIN_CARET_WIDTH;

      m_pimpl->context.length = length;
      if (m_pimpl->context.length < (m_pimpl->context.style.caretWidth + SLIDER_MIN_RUN_PIXELS))
        m_pimpl->context.length = m_pimpl->context.style.caretWidth + SLIDER_MIN_RUN_PIXELS;

      m_pimpl->context.value = a_value;
      if (m_pimpl->context.value < 0.0f)
        m_pimpl->context.value = 0.0f;
      else if (m_pimpl->context.value > 1.0f)
        m_pimpl->context.value = 1.0f;

      m_pimpl->context.position = a_position;
      m_pimpl->context.pSlider = this;

      m_pimpl->pState = new SliderStaticState(&m_pimpl->context, WidgetState::None);
    }

    SliderBase::~SliderBase()
    {
      delete m_pimpl;
    }

    Style::Slider const &SliderBase::GetDefaultStyle()
    {
      return s_style;
    }

    Style::Slider const &SliderBase::GetStyle() const
    {
      return m_pimpl->context.style;
    }

    void SliderBase::SetStyle(Style::Slider const &style)
    {
      m_pimpl->context.style = style;
    }

    void SliderBase::_HandleMessage(Message * a_pMsg)
    {
      SliderState *pNewState = m_pimpl->pState->HandleMessage(a_pMsg);
      if (pNewState != nullptr)
      {
        delete m_pimpl->pState;
        m_pimpl->pState = pNewState;
      }
    }

    void SliderBase::Draw()
    {
      int colourIndex = m_pimpl->pState->QueryState() == WidgetState::None ? (int)Style::Slider::s_Default : (int)Style::Slider::s_Hover;

      UIAABB viewableWindow;
      if (!GetGlobalViewableArea(viewableWindow))
        return;

      ::DgE::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());

      UIAABB lower, upper, caret;
      m_pimpl->context.GetInnerAABBs(lower, upper, caret);

      Colour lb = m_pimpl->context.style.colours[Style::Slider::c_LowerBar][colourIndex].face;
      Colour ub = m_pimpl->context.style.colours[Style::Slider::c_UpperBar][colourIndex].face;
      Colour c = m_pimpl->context.style.colours[Style::Slider::c_Caret][colourIndex].face;

      if (m_pimpl->context.style.components[Style::Slider::c_LowerBar].borderWidth == 0.0f)
        Renderer::DrawBox(lower, m_pimpl->context.style.colours[Style::Slider::c_LowerBar][colourIndex].face);
      else
        Renderer::DrawBoxWithBorder(lower,
          m_pimpl->context.style.components[Style::Slider::c_LowerBar].borderWidth,
          m_pimpl->context.style.colours[Style::Slider::c_LowerBar][colourIndex].face,
          m_pimpl->context.style.colours[Style::Slider::c_LowerBar][colourIndex].border);

      if (m_pimpl->context.style.components[Style::Slider::c_UpperBar].borderWidth == 0.0f)
        Renderer::DrawBox(upper, m_pimpl->context.style.colours[Style::Slider::c_UpperBar][colourIndex].face);
      else
        Renderer::DrawBoxWithBorder(upper,
          m_pimpl->context.style.components[Style::Slider::c_UpperBar].borderWidth,
          m_pimpl->context.style.colours[Style::Slider::c_UpperBar][colourIndex].face,
          m_pimpl->context.style.colours[Style::Slider::c_UpperBar][colourIndex].border);

      if (m_pimpl->context.style.components[Style::Slider::c_Caret].borderWidth == 0.0f)
        Renderer::DrawBox(caret, m_pimpl->context.style.colours[Style::Slider::c_Caret][colourIndex].face);
      else
        Renderer::DrawBoxWithBorder(lower,
          m_pimpl->context.style.components[Style::Slider::c_Caret].borderWidth,
          m_pimpl->context.style.colours[Style::Slider::c_Caret][colourIndex].face,
          m_pimpl->context.style.colours[Style::Slider::c_Caret][colourIndex].border);
    }

    void SliderBase::BindHoverOn(std::function<void()> a_fn)
    {
      m_pimpl->context.clbk_HoverOn = a_fn;
    }

    void SliderBase::BindHoverOff(std::function<void()> a_fn)
    {
      m_pimpl->context.clbk_HoverOff = a_fn;
    }

    WidgetState SliderBase::QueryState() const
    {
      return m_pimpl->pState->QueryState();
    }

    float SliderBase::SetNormalisedValue(float a_val)
    {
      if (a_val < 0.0f)
        a_val = 0.0f;
      else if (a_val > 1.0f)
        a_val = 1.0f;

      m_pimpl->context.value = a_val;
      return m_pimpl->context.value;
    }

    Widget * SliderBase::GetParent() const
    {
      return m_pimpl->context.pParent;
    }

    void SliderBase::SetParent(Widget * a_pParent)
    {
      m_pimpl->context.pParent = a_pParent;
    }

    void SliderBase::SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->context.position = a_pos;
    }

    void SliderBase::SetSize(vec2 const & a_size)
    {
      m_pimpl->context.length = a_size.x();
      if (m_pimpl->context.length < (m_pimpl->context.style.caretWidth + SLIDER_MIN_RUN_PIXELS))
        m_pimpl->context.length = (m_pimpl->context.style.caretWidth + SLIDER_MIN_RUN_PIXELS);
    }

    vec2 SliderBase::GetLocalPosition()
    {
      return m_pimpl->context.position;
    }

    vec2 SliderBase::GetSize()
    {
      float c = m_pimpl->context.style.components[Style::Slider::c_Caret].height;
      float l = m_pimpl->context.style.components[Style::Slider::c_LowerBar].height;
      float u = m_pimpl->context.style.components[Style::Slider::c_UpperBar].height;

      float h = c;
      if (l > h) h = l;
      if (u > h) h = u;

      int const i = m_pimpl->context.isVertical ? 1 : 0;
      int const j = (i + 1) & 1;

      vec2 result;
      result[i] = m_pimpl->context.length;
      result[j] = h;

      return result;
    }
  }
}