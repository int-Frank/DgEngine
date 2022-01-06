//@group GUI

#include "DgDoublyLinkedList.h"
#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Container.h"
#include "GUI_Button.h"
#include "Renderer.h"

#define CONTENT_MARGIN 2.0f

namespace DgE
{
  namespace GUI
  {
    //------------------------------------------------------------------------------------
    // Contaier context
    //------------------------------------------------------------------------------------

    class ContainerContext
    {
    public:

      ContainerContext()
        : style{}
        , pParent(nullptr)
        , pGrab(nullptr)
        , aabb{}
        , grabPressed(false)
        , children()
      {

      }

      ~ContainerContext()
      {
        Clear();
        delete pGrab;
      }

      void Clear()
      {
        for (auto pWgt : children)
          delete pWgt;
        children.clear();
      }

      Style::Container style;
      Container *pContainer;
      Widget *pParent;
      Button *pGrab;
      UIAABB aabb;
      bool grabPressed;
      Dg::DoublyLinkedList<Widget *> children;
    };

    //------------------------------------------------------------------------------------
    // State class declarations
    //------------------------------------------------------------------------------------

    class ContainerState
    {
    public:

      ContainerState(ContainerContext * a_pContext) : m_pContext(a_pContext) {}
      virtual ~ContainerState() {}

      virtual WidgetState QueryState() const = 0;
      virtual ContainerState * HandleMessage(Message *) = 0;

    protected:

      ContainerContext * m_pContext;
    };

    class StaticState : public ContainerState
    {
    public:

      StaticState(ContainerContext * a_pContext);
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

      MoveState(ContainerContext * a_pContext, vec2 const & a_controlAnchor);
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

      ResizeState(ContainerContext * a_pContext, vec2 const & a_controlAnchor);
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

    StaticState::StaticState(ContainerContext * a_pContext)
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

      if (m_pFocus != nullptr)
      {
        m_pFocus->HandleMessage(a_pMsg);
        if (m_pFocus->QueryState() == WidgetState::None)
        {
          m_pFocus = nullptr;
          m_state = WidgetState::None;
        }
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
          pResult = nullptr;
      }
      else if (a_pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
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
      if (!m_pContext->pContainer->GetGlobalAABB(aabb))
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

      for (auto it = m_pContext->children.begin(); it != m_pContext->children.end(); it++)
      {
        (*it)->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          if ((*it)->QueryState() == WidgetState::HasFocus)
          {
            m_pFocus = (*it);
            m_state = WidgetState::HasFocus;
          }
          if ((*it)->IsContainer())
          {
            Widget * pWgt = (*it);
            m_pContext->children.erase(it);
            m_pContext->children.push_front(pWgt); // We do this to render the container on top
          }
          return nullptr;
        }
      }

      if (m_pContext->pContainer->HasFlag(WidgetFlag::Movable) && PointInBox(point, aabb))
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new MoveState(m_pContext, point);
      }

      return nullptr;
    }

    ContainerState * StaticState::HandleMessage(Message_GUI_PointerUp * a_pMsg)
    {
      for (Widget * pWidget : m_pContext->children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          if (pWidget->QueryState() == WidgetState::HasFocus)
          {
            m_pFocus = pWidget;
            m_state = WidgetState::HasFocus;
          }
          break;
        }
      }
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

      for (Widget * pWidget : m_pContext->children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          if (pWidget->QueryState() == WidgetState::HasFocus)
          {
            m_pFocus = pWidget;
            m_state = WidgetState::HasFocus;
          }
          break;
        }
      }

      UIAABB aabb;
      if (!m_pContext->pContainer->GetGlobalAABB(aabb))
      {
        vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

        if (PointInBox(point, aabb))
        {

          a_pMsg->ConsumeHover();
        }
      }
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // MoveState
    //------------------------------------------------------------------------------------

    MoveState::MoveState(ContainerContext * a_pContext, vec2 const & a_controlAnchor)
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
        if (!m_pContext->pParent->GetGlobalAABB(aabb))
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

    ResizeState::ResizeState(ContainerContext * a_pContext, vec2 const & a_controlAnchor)
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

      if (newSize.x() < Container::s_minSize.x())
        newSize.x() = Container::s_minSize.x();

      if (newSize.y() < Container::s_minSize.y())
        newSize.y() = Container::s_minSize.y();

      m_pContext->aabb.size = newSize;
      a_pMsg->SetFlag(Message::Flag::Handled, true);

      if (m_pContext->pGrab != nullptr)
        m_pContext->pGrab->SetPosition(m_pContext->aabb.size - m_pContext->pGrab->GetSize());

      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // Container
    //------------------------------------------------------------------------------------

    vec2 const Container::s_minSize = vec2(50.f, 20.f);

    class Container::PIMPL
    {
    public:

      PIMPL() 
        : pState(nullptr)
        , context()
      {}

      ~PIMPL() { delete pState; }

      ContainerState *pState;
      ContainerContext context;
    };

    Container::Container(Widget * a_pParent, vec2 const a_position, vec2 const & a_size, Style::Container const & style, std::initializer_list<WidgetFlag> a_flags)
      : Widget({ WidgetFlag::NotResponsive,
                 WidgetFlag::StretchWidth,
                 WidgetFlag::StretchHeight,
                 WidgetFlag::Resizable,
                 WidgetFlag::Movable,
                 WidgetFlag::NoBackground
               }, a_flags)
      , m_pimpl(new PIMPL())
    {
      m_pimpl->context.pContainer = this;
      m_pimpl->context.pParent = a_pParent;
      m_pimpl->context.grabPressed = false;
      m_pimpl->context.pGrab = nullptr;
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

        m_pimpl->context.pGrab = Button::CreateWithGlyph(this, "\xEE\x80\x80", a_size - grabSize, grabSize, grabStyle);
        m_pimpl->context.pGrab->BindSelect([pBool = &m_pimpl->context.grabPressed](){*pBool = true; });
      }

      m_pimpl->pState = new StaticState(&m_pimpl->context);
    }

    Container::~Container()
    {
      delete m_pimpl;
    }

    Container * Container::Create(Widget * a_pParent, vec2 const a_position, vec2 const & a_size, std::initializer_list<WidgetFlag> a_flags)
    {
      return new Container(a_pParent, a_position, a_size, s_style, a_flags);
    }

    Container *Container::Create(Widget *a_pParent, vec2 const a_position, vec2 const &a_size, Style::Container const & style, std::initializer_list<WidgetFlag> a_flags)
    {
      return new Container(a_pParent, a_position, a_size, style, a_flags);
    }

    Style::Container const & Container::GetDefaultStyle()
    {
      return s_style;
    }

    Style::Container const &Container::GetStyle() const
    {
      return m_pimpl->context.style;
    }

    void Container::SetStyle(Style::Container const &style)
    {
      m_pimpl->context.style = style;
    }

    void Container::_HandleMessage(Message * a_pMsg)
    {
      ContainerState *pNewState = m_pimpl->pState->HandleMessage(a_pMsg);
      if (pNewState != nullptr)
      {
        delete m_pimpl->pState;
        m_pimpl->pState = pNewState;
      }
    }

    void Container::Clear()
    {
      m_pimpl->context.Clear();
    }

    void Container::Add(Widget * a_pWgt)
    {
      a_pWgt->SetParent(this);
      if (a_pWgt->IsContainer())
        m_pimpl->context.children.push_front(a_pWgt);
      else
        m_pimpl->context.children.push_back(a_pWgt);
    }

    void Container::Remove(Widget * a_pWgt)
    {
      for (auto it = m_pimpl->context.children.begin(); it != m_pimpl->context.children.end(); it++)
      {
        if (*it == a_pWgt)
        {
          delete *it;
          m_pimpl->context.children.erase(it);
          break;
        }
      }
    }

    void Container::Draw()
    {
      UIAABB viewableWindow;

      if (!GetGlobalAABB(viewableWindow))
        return;

      if (!HasFlag(WidgetFlag::NoBackground))
      {
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
      }

      for (auto it = m_pimpl->context.children.end(); it != m_pimpl->context.children.begin();)
      {
        it--;
        (*it)->Draw();
      }

      if (m_pimpl->context.pGrab != nullptr)
        m_pimpl->context.pGrab->Draw();
    }

    WidgetState Container::QueryState() const
    {
      return m_pimpl->pState->QueryState();
    }

    Widget * Container::GetParent() const
    {
      return m_pimpl->context.pParent;
    }

    void Container::SetParent(Widget * a_pParent)
    {
      m_pimpl->context.pParent = a_pParent;
    }

    void Container::_SetLocalPosition(vec2 const & a_pos)
    {
      m_pimpl->context.aabb.position = a_pos;
    }

    void Container::_SetSize(vec2 const & a_size)
    {
      m_pimpl->context.aabb.size = a_size;
    }

    vec2 Container::_GetLocalPosition()
    {
      return m_pimpl->context.aabb.position;
    }

    vec2 Container::_GetSize()
    {
      return m_pimpl->context.aabb.size;
    }

    bool Container::IsContainer() const
    {
      return true;
    }

    vec2 Container::GetContentDivPosition()
    {
      return vec2(CONTENT_MARGIN, CONTENT_MARGIN);
    }

    vec2 Container::GetContentDivSize()
    {
      vec2 size = GetSize() - vec2(CONTENT_MARGIN * 2.0f, CONTENT_MARGIN * 2.0f);

      if ((size[0] <= 0.0f) || (size[1] <= 0.0f))
        size.Zero();

      return size;
    }
  }
}