//@group GUI

#include "DgDoublyLinkedList.h"
#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Window.h"
#include "GUI_Button.h"
#include "GUI_Container.h"
#include "GUI_Slider.h"
#include "Renderer.h"

#define MIN_DIMENSION_X 32.0f
#define MIN_DIMENSION_Y 32.0f
#define MIN_CARET_SIZE 8.0f
#define WINDOW_CONTROLLER_BORDER 16.0f
#define GRAB_SIZE 14.0f
#define INNER_MARGIN 1.0f
#define SLIDER_MARGIN_FRONT 0.0f
#define SLIDER_MARGIN_END 0.0f

namespace DgE
{
  namespace GUI
  {
    //------------------------------------------------------------------------------------
    // Contaier context
    //------------------------------------------------------------------------------------

    class WindowContext
    {
    public:

      WindowContext()
        : style{}
        , pWindow(nullptr)
        , pParent(nullptr)
        , pWindowContainer(nullptr)
        , pViewableContainer(nullptr)
        , pUserContainer(nullptr)
        , pGrab(nullptr)
        , pSliders{nullptr, nullptr}
        , aabb{}
        , grabPressed(false)
      {

      }

      ~WindowContext()
      {
        delete pGrab;
        delete pWindowContainer;
      }

      void Clear()
      {
        pUserContainer->Clear();
      }

      Style::Slider GetSliderStyle() const
      {
        return {
          {
            {GRAB_SIZE, 0},
            {GRAB_SIZE, 0},
            {GRAB_SIZE - 2, 0},
          },
          {
            {
              {Window::s_style.sliderBack, 0},
              {Window::s_style.sliderBack, 0},
              {Window::s_style.sliderBack, 0},
            },
            {
              {Window::s_style.sliderBack, 0},
              {Window::s_style.sliderBack, 0},
              {Window::s_style.sliderBack, 0},
            },
            {
              {Window::s_style.sliderCaret, 0},
              {Window::s_style.sliderCaret, 0},
              {Window::s_style.sliderCaret, 0},
            }
          },
          1.0f
        };
      }

      void RemoveSlider(int i)
      {
        if (pSliders[i] == nullptr)
          return;

        pWindowContainer->Remove(pSliders[i]);
        pSliders[i] = nullptr;

        vec2 pos = pUserContainer->GetLocalPosition();
        pos[i] = 0.0f;
        pUserContainer->SetLocalPosition(pos);
      }

      vec2 GetUserContainerSize(bool hasSlider[2])
      {
        WidgetFlag const flags[2] = { WidgetFlag::HorizontalScroll, WidgetFlag::VerticalScroll };
        vec2 viewSize = pViewableContainer->GetSize();

        int i = 0;
        hasSlider[0] = false;
        hasSlider[1] = false;
        while (i < 2)
        {
          int j = (i + 1) & 1;

          bool overflow = pUserContainer->GetSize()[i] > viewSize[i];

          if (overflow && pWindow->HasFlag(flags[i]) && !hasSlider[i])
          {
            viewSize[j] -= (GRAB_SIZE + INNER_MARGIN);
            hasSlider[i] = true;
            i = j;
            continue;
          }

          i++;
        }
        return viewSize;
      }

      // Returns true if slider added
      void AddSlider(int i)
      {
        WidgetFlag const flags[2] = { WidgetFlag::HorizontalScroll, WidgetFlag::VerticalScroll };
        int j = (i + 1) & 1;

        float sliderLength = pWindowContainer->GetSize()[i] - SLIDER_MARGIN_FRONT - SLIDER_MARGIN_END;
        if (pGrab != nullptr || pWindow->HasFlag(flags[j]))
          sliderLength -= GRAB_SIZE;

        if (pSliders[i] == nullptr)
        {
          pSliders[i] = SliderFloat::Create(vec2(0.0f, 0.0f), sliderLength, 0.0f, 1.0f, i == 0 ? 0.0f : 1.0f, i == 1, GetSliderStyle());
          pSliders[i]->BindNewValue([i, pContext = this](float val) {pContext->SliderCallback(i, val); });
          pWindowContainer->Add(pSliders[i]);
        }
        else
        {
          pSliders[i]->SetSize(vec2(sliderLength, 0.0f));
          pSliders[i]->SetValue(pSliders[i]->GetValue());
        }

        vec2 sliderPosition;
        sliderPosition[i] = SLIDER_MARGIN_FRONT;
        sliderPosition[j] = pWindowContainer->GetSize()[j] - GRAB_SIZE;

        pWindowContainer->Move(pSliders[i], sliderPosition);

        Style::Slider style = GetSliderStyle();
        style.caretWidth = pViewableContainer->GetSize()[i] / pUserContainer->GetSize()[i] * sliderLength;

        if (style.caretWidth < MIN_CARET_SIZE)
          style.caretWidth = MIN_CARET_SIZE;

        pSliders[i]->SetStyle(style);
      }

      void BuildWindow()
      {
        vec2 border(INNER_MARGIN, INNER_MARGIN);
        pWindowContainer->SetSize(aabb.size - 2.0f * (vec2(style.borderWidth, style.borderWidth) + border));
        pViewableContainer->SetSize(pWindowContainer->GetSize());

        if (pGrab != nullptr)
          pWindowContainer->Move(pGrab, pWindowContainer->GetSize() - pGrab->GetSize());

        pUserContainer->FitSizeToContent(pViewableContainer->GetSize());

        bool hasSlider[2] = {false, false};
        pViewableContainer->SetSize(GetUserContainerSize(hasSlider));

        for (int i = 0; i < 2; i++)
        {
          int j = (i + 1) & 1;
          if (hasSlider[i])
            AddSlider(i);
          else
            RemoveSlider(i);
        }

        if (pGrab != nullptr)
        {
          Style::Button grabStyle = pGrab->GetStyle();
          Colour clr = 0;
          if (pSliders[0] != nullptr || pSliders[1] != nullptr)
            clr = Window::s_style.sliderBack;
          grabStyle.colours[Style::Button::Default].face = clr;
          grabStyle.colours[Style::Button::Hover].face = clr;
          pGrab->SetStyle(grabStyle);
        }
      }

      void SliderCallback(int i, float val)
      {
        float userSize = pUserContainer->GetSize()[i];
        float viewSize = pViewableContainer->GetSize()[i];
        if (userSize <= viewSize)
          return;

        float a = i == 0 ? val : 1.0f - val;
        float offset = (userSize - viewSize) * a;
        vec2 pos = pUserContainer->GetLocalPosition();
        pos[i] = -offset;
        pUserContainer->SetLocalPosition(pos);
      }

      Style::Window style;
      Window *pWindow;
      Widget *pParent;
      Container *pWindowContainer;
      Container *pViewableContainer;
      Container *pUserContainer;
      Button *pGrab;
      SliderFloat *pSliders[2]; // horizontal, vertical
      UIAABB aabb;
      bool grabPressed;
    };

    //------------------------------------------------------------------------------------
    // State class declarations
    //------------------------------------------------------------------------------------

    class ContainerState
    {
    public:

      ContainerState(WindowContext * a_pContext) : m_pContext(a_pContext) {}
      virtual ~ContainerState() {}

      virtual WidgetState QueryState() const = 0;
      virtual ContainerState * HandleMessage(Message *) = 0;

    protected:

      WindowContext * m_pContext;
    };

    class StaticState : public ContainerState
    {
    public:

      StaticState(WindowContext * a_pContext);
      ~StaticState() {}

      WidgetState QueryState() const override;

      ContainerState * HandleMessage(Message * a_pMsg) override;
      ContainerState * HandleMessage(Message_GUI_PointerDown * a_pMsg);
      ContainerState * HandleMessage(Message_GUI_PointerUp * a_pMsg);
      ContainerState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      Widget * m_pFocus;
      WidgetState m_state;
    };

    class MoveState : public ContainerState
    {
    public:

      MoveState(WindowContext * a_pContext, vec2 const & a_controlAnchor);
      ~MoveState() {}

      WidgetState QueryState() const override;

      ContainerState * HandleMessage(Message * a_pMsg) override;
      ContainerState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      vec2 m_controlAnchor;
      vec2 m_positionAnchor;
    };

    class ResizeState : public ContainerState
    {
    public:

      ResizeState(WindowContext * a_pContext, vec2 const & a_controlAnchor);
      ~ResizeState() {}

      WidgetState QueryState() const override;

      ContainerState * HandleMessage(Message *) override;
      ContainerState * HandleMessage(Message_GUI_PointerMove *);

    private:

      vec2 m_controlAnchor;
      vec2 m_sizeAnchor;
    };

    //------------------------------------------------------------------------------------
    // StaticState
    //------------------------------------------------------------------------------------

    StaticState::StaticState(WindowContext * a_pContext)
      : ContainerState(a_pContext)
      , m_pFocus(nullptr)
      , m_state(WidgetState::None)
    {

    }

    WidgetState StaticState::QueryState() const
    {
      return m_state;
    }

    ContainerState * StaticState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      ContainerState * pResult = nullptr;

      m_pContext->pWindowContainer->HandleMessage(a_pMsg);
      if (a_pMsg->QueryFlag(Message::Flag::Handled))
      {
        if (m_pContext->grabPressed)
        {
          m_pContext->grabPressed = false;
          Message_GUI_PointerDown* pMsg = dynamic_cast<Message_GUI_PointerDown*>(a_pMsg);
          vec2 point((float)pMsg->x, (float)pMsg->y);
          return new ResizeState(m_pContext, point);
        }
        return nullptr;
      }

      if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
      {
        pResult = HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(a_pMsg));
      }
      else if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        pResult = HandleMessage(dynamic_cast<Message_GUI_PointerUp *>(a_pMsg));
      }
      else if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
      {
        pResult = HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));
      }
      return pResult;
    }

    ContainerState * StaticState::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!m_pContext->pWindow->GetGlobalViewableArea(aabb))
        return nullptr;

      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (!PointInBox(point, aabb))
        return nullptr;

      if (m_pContext->pWindow->HasFlag(WidgetFlag::Movable) && PointInBox(point, aabb))
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new MoveState(m_pContext, point);
      }

      return nullptr;
    }

    ContainerState * StaticState::HandleMessage(Message_GUI_PointerUp * a_pMsg)
    {
      return nullptr;
    }

    ContainerState * StaticState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      UIAABB aabb;
      if (!m_pContext->pWindow->GetGlobalViewableArea(aabb))
      {
        vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

        if (PointInBox(point, aabb))
          a_pMsg->ConsumeHover();
      }
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // MoveState
    //------------------------------------------------------------------------------------

    MoveState::MoveState(WindowContext * a_pContext, vec2 const & a_controlAnchor)
      : ContainerState(a_pContext)
      , m_controlAnchor(a_controlAnchor)
      , m_positionAnchor(a_pContext->aabb.position)
    {

    }

    WidgetState MoveState::QueryState() const
    {
      return WidgetState::HasFocus;
    }

    ContainerState * MoveState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new StaticState(m_pContext);
      }

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

      return nullptr;
    }

    ContainerState * MoveState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (m_pContext->pParent)
      {
        UIAABB aabb;
        if (!m_pContext->pParent->GetGlobalViewableArea(aabb))
          return nullptr;

        if (PointInBox(point, aabb))
          m_pContext->aabb.position = m_positionAnchor + (point - m_controlAnchor);
      }
      else
      {
        m_pContext->aabb.position = m_positionAnchor + (point - m_controlAnchor);
      }

      a_pMsg->SetFlag(Message::Flag::Handled, true);
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // ResizeState
    //------------------------------------------------------------------------------------

    ResizeState::ResizeState(WindowContext * a_pContext, vec2 const & a_controlAnchor)
      : ContainerState(a_pContext)
      , m_controlAnchor(a_controlAnchor)
      , m_sizeAnchor(a_pContext->aabb.size)
    {

    }

    WidgetState ResizeState::QueryState() const
    {
      return WidgetState::HasFocus;
    }

    ContainerState * ResizeState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new StaticState(m_pContext);
      }

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

      return nullptr;
    }

    ContainerState * ResizeState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);
      vec2 newSize = m_sizeAnchor + (point - m_controlAnchor);

      if (newSize.x() < MIN_DIMENSION_X)
        newSize.x() = MIN_DIMENSION_X;

      if (newSize.y() < MIN_DIMENSION_Y)
        newSize.y() = MIN_DIMENSION_Y;

      m_pContext->aabb.size = newSize;
      a_pMsg->SetFlag(Message::Flag::Handled, true);

      m_pContext->BuildWindow();

      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // Window
    //------------------------------------------------------------------------------------

    class Window::PIMPL
    {
    public:

      PIMPL() 
        : pState(nullptr)
        , context()
      {}

      ~PIMPL() { delete pState; }

      ContainerState *pState;
      WindowContext context;
    };

    Window::Window(vec2 const a_position, vec2 const & a_size, Style::Window const & style, std::initializer_list<WidgetFlag> a_flags)
      : Widget({ WidgetFlag::NotResponsive,
                 WidgetFlag::StretchWidth,
                 WidgetFlag::StretchHeight,
                 WidgetFlag::Resizable,
                 WidgetFlag::VerticalScroll,
                 WidgetFlag::HorizontalScroll,
                 WidgetFlag::Movable
               }, a_flags)
      , m_pimpl(new PIMPL())
    {
      m_pimpl->context.pWindow = this;

      SetStyle(style);
      vec2 border(style.borderWidth + INNER_MARGIN, style.borderWidth + INNER_MARGIN);
      m_pimpl->context.aabb = {a_position, a_size};
      m_pimpl->context.pWindowContainer = Container::Create(border, a_size - 2.0f * border);
      m_pimpl->context.pWindowContainer->SetParent(this);

      if (HasFlag(WidgetFlag::Resizable))
      {
        vec2 grabSize(GRAB_SIZE, GRAB_SIZE);
        Style::Button grabStyle = 
        {
          {
            {0xFF535250, 0},
            {0xFF535250, 0}
          },
          {
            {
              style.grabButton,
              Style::Text::HorizontalAlignment::Left,
              Style::Text::VerticalAlignment::Top,
              1.0f,
              true
            },
            {
              style.grabButtonHover,
              Style::Text::HorizontalAlignment::Left,
              Style::Text::VerticalAlignment::Top,
              1.0f,
              true
            }
          },
          0.0f,
          { 2.0f, 2.0f }
        };

        m_pimpl->context.pGrab = Button::CreateWithGlyph("\xEE\x80\x81", a_size - grabSize - border, grabSize, grabStyle);
        m_pimpl->context.pGrab->BindSelect([pBool = &m_pimpl->context.grabPressed](){*pBool = true; });
        m_pimpl->context.pWindowContainer->Add(m_pimpl->context.pGrab);
      }

      m_pimpl->pState = new StaticState(&m_pimpl->context);

      m_pimpl->context.pViewableContainer = Container::Create(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f));
      m_pimpl->context.pUserContainer = Container::Create(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f));
      m_pimpl->context.pViewableContainer->Add(m_pimpl->context.pUserContainer);
      m_pimpl->context.pWindowContainer->Add(m_pimpl->context.pViewableContainer);
      m_pimpl->context.BuildWindow();
    }

    Window::~Window()
    {
      delete m_pimpl;
    }

    Window * Window::Create(vec2 const a_position, vec2 const & a_size, std::initializer_list<WidgetFlag> a_flags)
    {
      return new Window(a_position, a_size, s_style, a_flags);
    }

    Window *Window::Create(vec2 const a_position, vec2 const &a_size, Style::Window const & style, std::initializer_list<WidgetFlag> a_flags)
    {
      return new Window(a_position, a_size, style, a_flags);
    }

    Style::Window const & Window::GetDefaultStyle()
    {
      return s_style;
    }

    Style::Window const &Window::GetStyle() const
    {
      return m_pimpl->context.style;
    }

    void Window::SetStyle(Style::Window const &style)
    {
      m_pimpl->context.style = style;
    }

    void Window::_HandleMessage(Message * a_pMsg)
    {
      ContainerState *pNewState = m_pimpl->pState->HandleMessage(a_pMsg);
      if (pNewState != nullptr)
      {
        delete m_pimpl->pState;
        m_pimpl->pState = pNewState;
      }
    }

    void Window::Clear()
    {
      m_pimpl->context.Clear();
      m_pimpl->context.BuildWindow();
    }

    void Window::Add(Widget * a_pWgt)
    {
      m_pimpl->context.pUserContainer->Add(a_pWgt);
      m_pimpl->context.BuildWindow();
    }

    void Window::Remove(Widget * a_pWgt)
    {
      m_pimpl->context.pUserContainer->Remove(a_pWgt);
      m_pimpl->context.BuildWindow();
    }

    void Window::Draw()
    {
      UIAABB viewableWindow;

      if (!GetGlobalViewableArea(viewableWindow))
        return;

      vec2 size = GetSize() - 2.0f * vec2(m_pimpl->context.style.borderWidth, m_pimpl->context.style.borderWidth);
      if (size.x() < 0.0f)
        size.x() = 0.0f;
      if (size.y() < 0.0f)
        size.y() = 0.0f;

      vec2 pos = GetGlobalPosition() + vec2(m_pimpl->context.style.borderWidth, m_pimpl->context.style.borderWidth);

      ::DgE::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());

      if (m_pimpl->context.style.borderWidth == 0.0f)
        Renderer::DrawBox({pos, size}, m_pimpl->context.style.face);
      else
        Renderer::DrawBoxWithBorder({pos, size}, m_pimpl->context.style.borderWidth, m_pimpl->context.style.face, m_pimpl->context.style.border);
      
      m_pimpl->context.pWindowContainer->Draw();
    }

    WidgetState Window::QueryState() const
    {
      return m_pimpl->pState->QueryState();
    }

    Widget * Window::GetParent() const
    {
      return m_pimpl->context.pParent;
    }

    void Window::SetParent(Widget * a_pParent)
    {
      m_pimpl->context.pParent = a_pParent;
    }

    void Window::SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->context.aabb.position = a_pos;
    }

    void Window::SetSize(vec2 const & a_size)
    {
      m_pimpl->context.aabb.size = a_size;
      m_pimpl->context.BuildWindow();
    }

    vec2 Window::GetLocalPosition()
    {
      return m_pimpl->context.aabb.position;
    }

    vec2 Window::GetSize()
    {
      return m_pimpl->context.aabb.size;
    }

    bool Window::IsContainer() const
    {
      return true;
    }
  }
}