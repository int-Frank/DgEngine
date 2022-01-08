//@group GUI

#include "DgDoublyLinkedList.h"
#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Window.h"
#include "GUI_Button.h"
#include "GUI_Container.h"
#include "GUI_Slider.h"
#include "Renderer.h"

#define INNER_MARGIN 2.0f

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
        , pViewable(nullptr)
        , pContainer(nullptr)
        , pGrab(nullptr)
        , pSliderVertical(nullptr)
        , pSliderHorizontal(nullptr)
        , aabb{}
        , grabPressed(false)
      {

      }

      ~WindowContext()
      {
        delete pGrab;
        delete pSliderVertical;
        delete pSliderHorizontal;
        delete pContainer;
      }

      void Clear()
      {
        pContainer->Clear();
      }

      UIAABB GetInnerAABB()
      {
        float x = 0.0f;
        float y = 0.0f;

        if (pGrab != nullptr)
        {
          x = pGrab->GetSize().x();
          y = pGrab->GetSize().y();
        }

        if (pSliderVertical != nullptr && pSliderVertical->GetSize().x() > x)
          x = pSliderVertical->GetSize().x();

        if (pSliderHorizontal != nullptr && pSliderHorizontal->GetSize().y() > y)
          y = pSliderVertical->GetSize().y();

        UIAABB aabb = {
          vec2(INNER_MARGIN, INNER_MARGIN),
          pWindow->GetSize() - vec2(x + INNER_MARGIN, y + INNER_MARGIN)};
        return aabb;
      }

      Style::Window style;
      Window *pWindow;
      Widget *pParent;
      Container *pViewable;
      Container *pContainer;
      Button *pGrab;
      SliderFloat *pSliderVertical;
      SliderFloat *pSliderHorizontal;
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

      m_pContext->pContainer->HandleMessage(a_pMsg);
      if (a_pMsg->QueryFlag(Message::Flag::Handled))
        return nullptr;

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

      if (m_pContext->pGrab != nullptr)
      {
        Widget * pWgt(m_pContext->pGrab);
        pWgt->HandleMessage(a_pMsg);

        if (m_pContext->grabPressed)
        {
          m_pContext->grabPressed = false;
          a_pMsg->SetFlag(Message::Flag::Handled, true);
          return new ResizeState(m_pContext, point);
        }
      }

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

      if (m_pContext->pGrab != nullptr)
      {
        Widget * pWgt(m_pContext->pGrab);
        pWgt->HandleMessage(a_pMsg);
      }

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

      if (newSize.x() < Window::s_minSize.x())
        newSize.x() = Window::s_minSize.x();

      if (newSize.y() < Window::s_minSize.y())
        newSize.y() = Window::s_minSize.y();

      m_pContext->aabb.size = newSize;
      a_pMsg->SetFlag(Message::Flag::Handled, true);

      if (m_pContext->pGrab != nullptr)
        m_pContext->pGrab->SetPosition(m_pContext->aabb.size - m_pContext->pGrab->GetSize());

      UIAABB innerAABB = m_pContext->GetInnerAABB();
      m_pContext->pViewable->SetSize(innerAABB.size);

      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // Window
    //------------------------------------------------------------------------------------

    vec2 const Window::s_minSize = vec2(50.f, 20.f);

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
      m_pimpl->context.grabPressed = false;
      m_pimpl->context.pGrab = nullptr;

      if (HasFlag(WidgetFlag::VerticalScroll))
      {
        //Add slider
      }

      if (HasFlag(WidgetFlag::HorizontalScroll))
      {
        //Add slider
      }

      SetStyle(style);

      m_pimpl->context.aabb = {a_position, a_size};

      if (HasFlag(WidgetFlag::Resizable))
      {
        vec2 grabSize(12.0f, 12.0f);
        Style::Button grabStyle = 
        {
          {
            {0, 0},
            {0, 0}
          },
          {
            {
              style.grabButton,
              Style::Text::HorizontalAlignment::Centre,
              Style::Text::VerticalAlignment::Top,
              1.0f,
              true
            },
            {
              style.grabButtonHover,
              Style::Text::HorizontalAlignment::Centre,
              Style::Text::VerticalAlignment::Top,
              1.0f,
              true
            }
          },
          0.0f,
          2.0f
        };

        m_pimpl->context.pGrab = Button::CreateWithGlyph("\xEE\x80\x80", a_size - grabSize, grabSize, grabStyle);
        m_pimpl->context.pGrab->SetParent(this);
        m_pimpl->context.pGrab->BindSelect([pBool = &m_pimpl->context.grabPressed](){*pBool = true; });
      }

      m_pimpl->pState = new StaticState(&m_pimpl->context);

      UIAABB innerAABB = m_pimpl->context.GetInnerAABB();
      m_pimpl->context.pViewable = Container::Create(innerAABB.position, innerAABB.size);
      m_pimpl->context.pContainer = Container::Create(vec2(0.0f, 0.0f), m_pimpl->context.aabb.size);
      m_pimpl->context.pViewable->SetParent(this);
      m_pimpl->context.pViewable->Add(m_pimpl->context.pContainer);
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
    }

    void Window::Add(Widget * a_pWgt)
    {
      m_pimpl->context.pContainer->Add(a_pWgt);
    }

    void Window::Remove(Widget * a_pWgt)
    {
      m_pimpl->context.pContainer->Remove(a_pWgt);
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
      
      m_pimpl->context.pContainer->Draw();

      if (m_pimpl->context.pGrab != nullptr)
        m_pimpl->context.pGrab->Draw();
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

    void Window::_SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->context.aabb.position = a_pos;
    }

    void Window::_SetSize(vec2 const & a_size)
    {
      m_pimpl->context.aabb.size = a_size;
    }

    vec2 Window::_GetLocalPosition()
    {
      return m_pimpl->context.aabb.position;
    }

    vec2 Window::_GetSize()
    {
      return m_pimpl->context.aabb.size;
    }

    bool Window::IsContainer() const
    {
      return true;
    }
  }
}