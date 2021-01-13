//@group GUI

#include "DgDoublyLinkedList.h"
#include "GUI.h"
#include "GUI_Internal.h"
#include "GUI_Container.h"
#include "GUI_Button.h"
#include "Renderer.h"

#define CONTAINER_GRAB_SIZE 16.0f

namespace Engine
{
  namespace GUI
  {
    //------------------------------------------------------------------------------------
    // State class declarations
    //------------------------------------------------------------------------------------
    class Container::InternalState
    {
    public:

      struct Data
      {
        Container * pContainer;
        Widget * pParent;
        Button * pGrab;
        Colour clr[(int)ContainerElement::COUNT];
        UIAABB aabb;
        float contentMargin;
        float outlineWidth;
        bool grabPressed;
        Dg::DoublyLinkedList<Widget *> children;
      };

      InternalState(Data * a_pData);
      virtual ~InternalState();

      void Destroy();

      virtual WidgetState QueryState() const = 0;
      Widget * GetParent() const;
      void SetParent(Widget * a_pParent);

      void Clear();

      void SetColour(ContainerElement, Colour);
      void SetContentMargin(float);

      void Add(Widget * a_pWgt);
      void Remove(Widget * a_pWgt);

      void Draw();
      vec2 GetContentDivPosition();
      vec2 GetContentDivSize();

      virtual InternalState * HandleMessage(Message *) = 0;

      void _SetLocalPosition(vec2 const &);
      void _SetSize(vec2 const &);
      vec2 _GetLocalPosition();
      vec2 _GetSize();

    protected:

      Data * m_pData;
    };

    class StaticState : public Container::InternalState
    {
    public:

      StaticState(Data * a_pData);
      ~StaticState();

      WidgetState QueryState() const override;

      InternalState * HandleMessage(Message * a_pMsg) override;
      InternalState * HandleMessage(Message_GUI_PointerDown * a_pMsg);
      InternalState * HandleMessage(Message_GUI_PointerUp * a_pMsg);
      InternalState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      Widget * m_pFocus;
      WidgetState m_state;
    };

    class MoveState : public Container::InternalState
    {
    public:

      MoveState(Data * a_pData, vec2 const & a_controlAnchor);
      ~MoveState();

      WidgetState QueryState() const override;

      InternalState * HandleMessage(Message * a_pMsg) override;
      InternalState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

    private:

      vec2 m_controlAnchor;
      vec2 m_positionAnchor;
    };

    class ResizeState : public Container::InternalState
    {
    public:

      ResizeState(Data * a_pData, vec2 const & a_controlAnchor);
      ~ResizeState();

      WidgetState QueryState() const override;

      InternalState * HandleMessage(Message *) override;
      InternalState * HandleMessage(Message_GUI_PointerMove *);

    private:

      vec2 m_controlAnchor;
      vec2 m_sizeAnchor;
    };

    //------------------------------------------------------------------------------------
    // InternalState
    //------------------------------------------------------------------------------------

    Container::InternalState::InternalState(InternalState::Data * a_pData)
      : m_pData(a_pData)
    {

    }

    Container::InternalState::~InternalState()
    {

    }

    void Container::InternalState::Destroy()
    {
      Clear();

      delete m_pData->pGrab;
      delete m_pData;
      m_pData = nullptr;
    }

    Widget * Container::InternalState::GetParent() const
    {
      return m_pData->pParent;
    }

    void Container::InternalState::SetParent(Widget * a_pParent)
    {
      m_pData->pParent = a_pParent;
    }

    vec2 Container::InternalState::_GetSize()
    {
      return m_pData->aabb.size;
    }

    void Container::InternalState::Clear()
    {
      for (auto pWgt : m_pData->children)
        delete pWgt;
      m_pData->children.clear();
    }

    void Container::InternalState::Add(Widget * a_pWgt)
    {
      if (a_pWgt->IsContainer())
        m_pData->children.push_front(a_pWgt);
      else
        m_pData->children.push_back(a_pWgt);
    }

    void Container::InternalState::Remove(Widget * a_pWgt)
    {
      for (auto it = m_pData->children.begin(); it != m_pData->children.end(); it++)
      {
        if (*it == a_pWgt)
        {
          delete * it;
          m_pData->children.erase(it);
          break;
        }
      }
    }

    void Container::InternalState::Draw()
    {
      UIAABB viewableWindow;
      if (!m_pData->pContainer->GetGlobalAABB(viewableWindow))
        return;

      if (!m_pData->pContainer->HasFlag(WidgetFlag::NoBackground))
      {
        vec2 size = m_pData->pContainer->GetSize() - 2.0f * vec2(m_pData->outlineWidth, m_pData->outlineWidth);
        if (size.x() < 0.0f)
          size.x() = 0.0f;
        if (size.y() < 0.0f)
          size.y() = 0.0f;

        vec2 pos = m_pData->pContainer->GetGlobalPosition() + vec2(m_pData->outlineWidth, m_pData->outlineWidth);

        ::Engine::Renderer::SetSissorBox((int)viewableWindow.position.x(), (int)viewableWindow.position.y(), (int)viewableWindow.size.x(), (int)viewableWindow.size.y());
        Renderer::DrawBoxWithOutline({pos, size}, m_pData->outlineWidth, m_pData->clr[(int)ContainerElement::Face], m_pData->clr[(int)ContainerElement::Outline]);
      }

      for (auto it = m_pData->children.end();;)
      {
        if (it == m_pData->children.begin())
          break;
        it--;
        (*it)->Draw();
      }

      if (m_pData->pGrab != nullptr)
        m_pData->pGrab->Draw();
    }

    vec2 Container::InternalState::_GetLocalPosition()
    {
      return m_pData->aabb.position;
    }

    void Container::InternalState::_SetLocalPosition(vec2 const & a_position)
    {
      m_pData->aabb.position = a_position;
    }

    void Container::InternalState::_SetSize(vec2 const & a_size)
    {
      m_pData->aabb.size = a_size;
    }

    void Container::InternalState::SetColour(ContainerElement a_ele, Colour a_clr)
    {
      BSR_ASSERT(a_ele != ContainerElement::COUNT);

      if ((a_ele == ContainerElement::Grab) && (m_pData->pGrab != nullptr))
        m_pData->pGrab->SetColour(ButtonState::Normal, ButtonElement::Face, a_clr);
      else if ((a_ele == ContainerElement::GrabHover) && (m_pData->pGrab != nullptr))
        m_pData->pGrab->SetColour(ButtonState::Hover, ButtonElement::Face, a_clr);

      m_pData->clr[(int)a_ele] = a_clr;
    }

    vec2 Container::InternalState::GetContentDivPosition()
    {
      return vec2(m_pData->contentMargin, m_pData->contentMargin);
    }

    vec2 Container::InternalState::GetContentDivSize()
    {
      vec2 size = m_pData->pContainer->GetSize() - vec2(m_pData->contentMargin * 2.0f,
                                                        m_pData->contentMargin * 2.0f);

      if ((size[0] <= 0.0f) || (size[1] <= 0.0f))
        size.Zero();

      return size;
    }

    void Container::InternalState::SetContentMargin(float a_size)
    {
      m_pData->contentMargin = a_size;
    }

    //------------------------------------------------------------------------------------
    // StaticState
    //------------------------------------------------------------------------------------

    StaticState::StaticState(Data * a_pData)
      : InternalState(a_pData)
      , m_pFocus(nullptr)
      , m_state(WidgetState::None)
    {

    }

    StaticState::~StaticState()
    {

    }

    WidgetState StaticState::QueryState() const
    {
      return m_state;
    }

    Container::InternalState * StaticState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      InternalState * pResult = nullptr;

      if (m_pFocus != nullptr)
      {
        m_pFocus->HandleMessage(a_pMsg);
        if (m_pFocus->QueryState() == WidgetState::None)
        {
          m_pFocus = nullptr;
          m_state = WidgetState::None;
        }
        if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
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

    Container::InternalState * StaticState::HandleMessage(Message_GUI_PointerDown * a_pMsg)
    {
      UIAABB aabb;
      if (!m_pData->pContainer->GetGlobalAABB(aabb))
        return nullptr;

      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (!PointInBox(point, aabb))
        return nullptr;

      if (m_pData->pGrab != nullptr)
      {
        Widget * pWgt(m_pData->pGrab);
        pWgt->HandleMessage(a_pMsg);

        if (m_pData->grabPressed)
        {
          m_pData->grabPressed = false;
          a_pMsg->SetFlag(Message::Flag::Handled, true);
          return new ResizeState(m_pData, point);
        }
      }

      for (auto it = m_pData->children.begin(); it != m_pData->children.end(); it++)
      {
        (*it)->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
        {
          if ((*it)->QueryState() == WidgetState::HasFocus)
          {
            m_pFocus = (*it);
            m_state = WidgetState::HasFocus;
          }
          if ((*it)->IsContainer())
          {
            Widget * pWgt = (*it);
            m_pData->children.erase(it);
            m_pData->children.push_front(pWgt);
          }
          return nullptr;
        }
      }

      if (m_pData->pContainer->HasFlag(WidgetFlag::Movable) && PointInBox(point, aabb))
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new MoveState(m_pData, point);
      }

      return nullptr;
    }

    Container::InternalState * StaticState::HandleMessage(Message_GUI_PointerUp * a_pMsg)
    {
      for (Widget * pWidget : m_pData->children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
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

    Container::InternalState * StaticState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (m_pData->pGrab != nullptr)
      {
        Widget * pWgt(m_pData->pGrab);
        pWgt->HandleMessage(a_pMsg);
      }

      for (Widget * pWidget : m_pData->children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
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
      if (!m_pData->pContainer->GetGlobalAABB(aabb))
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

    MoveState::MoveState(Data * a_pData, vec2 const & a_controlAnchor)
      : InternalState(a_pData)
      , m_controlAnchor(a_controlAnchor)
      , m_positionAnchor(a_pData->aabb.position)
    {

    }

    MoveState::~MoveState()
    {

    }

    WidgetState MoveState::QueryState() const
    {
      return WidgetState::HasFocus;
    }

    Container::InternalState * MoveState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new StaticState(m_pData);
      }

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

      return nullptr;
    }

    Container::InternalState * MoveState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (m_pData->pParent)
      {
        UIAABB aabb;
        if (!m_pData->pParent->GetGlobalAABB(aabb))
          return nullptr;

        if (PointInBox(point, aabb))
          m_pData->aabb.position = m_positionAnchor + (point - m_controlAnchor);
      }
      else
      {
        m_pData->aabb.position = m_positionAnchor + (point - m_controlAnchor);
      }

      a_pMsg->SetFlag(Message::Flag::Handled, true);
      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // ResizeState
    //------------------------------------------------------------------------------------

    ResizeState::ResizeState(Data * a_pData, vec2 const & a_controlAnchor)
      : InternalState(a_pData)
      , m_controlAnchor(a_controlAnchor)
      , m_sizeAnchor(a_pData->aabb.size)
    {

    }

    ResizeState::~ResizeState()
    {

    }

    WidgetState ResizeState::QueryState() const
    {
      return WidgetState::HasFocus;
    }

    Container::InternalState * ResizeState::HandleMessage(Message * a_pMsg)
    {
      if (a_pMsg->GetCategory() != MC_GUI)
        return nullptr;

      if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      {
        a_pMsg->SetFlag(Message::Flag::Handled, true);
        return new StaticState(m_pData);
      }

      if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

      return nullptr;
    }

    Container::InternalState * ResizeState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);
      vec2 newSize = m_sizeAnchor + (point - m_controlAnchor);

      if (newSize.x() < Container::s_minSize.x())
        newSize.x() = Container::s_minSize.x();

      if (newSize.y() < Container::s_minSize.y())
        newSize.y() = Container::s_minSize.y();

      m_pData->aabb.size = newSize;
      a_pMsg->SetFlag(Message::Flag::Handled, true);

      if (m_pData->pGrab != nullptr)
        m_pData->pGrab->SetPosition(m_pData->aabb.size - m_pData->pGrab->GetSize());

      return nullptr;
    }

    //------------------------------------------------------------------------------------
    // Container
    //------------------------------------------------------------------------------------

    vec2 const Container::s_minSize = vec2(50.f, 20.f);

    Container::Container(Widget * a_pParent, vec2 const a_position, vec2 const & a_size, std::initializer_list<WidgetFlag> a_flags)
      : Widget({ WidgetFlag::NotResponsive,
                 WidgetFlag::StretchWidth,
                 WidgetFlag::StretchHeight,
                 WidgetFlag::Resizable,
                 WidgetFlag::Movable,
                 WidgetFlag::NoBackground
               }, a_flags)
      , m_pState(nullptr)
    {
      InternalState::Data * pData = new InternalState::Data();
      pData->pContainer = this;
      pData->pParent = a_pParent;
      pData->grabPressed = false;
      pData->pGrab = nullptr;

      if (HasFlag(WidgetFlag::Resizable))
      {
        vec2 grabSize(CONTAINER_GRAB_SIZE, CONTAINER_GRAB_SIZE);
        pData->pGrab = Button::Create(this, "\xEE\x80\x80", a_size - grabSize, grabSize);
        pData->pGrab->SetContentMargin(-4.0f);
        pData->pGrab->BindSelect([pBool = &pData->grabPressed](){*pBool = true; });
        pData->pGrab->SetColour(ButtonState::Normal, ButtonElement::Face, 0);
        pData->pGrab->SetColour(ButtonState::Normal, ButtonElement::Outline, 0);
        pData->pGrab->SetColour(ButtonState::Normal, ButtonElement::Text, GetStyle().colours[col_ContainerGrab]);
        pData->pGrab->SetColour(ButtonState::Hover, ButtonElement::Face, 0);
        pData->pGrab->SetColour(ButtonState::Hover, ButtonElement::Outline, 0);
        pData->pGrab->SetColour(ButtonState::Hover, ButtonElement::Text, GetStyle().colours[col_ContainerGrabHover]);
      }

      pData->clr[(int)ContainerElement::Face] = GetStyle().colours[col_ContainerFace];
      pData->clr[(int)ContainerElement::Outline] = GetStyle().colours[col_ContainerOutline];
      pData->clr[(int)ContainerElement::Grab] = GetStyle().colours[col_ContainerGrab];
      pData->clr[(int)ContainerElement::GrabHover] = GetStyle().colours[col_ContainerGrabHover];

      pData->aabb ={a_position, a_size};
      pData->contentMargin = GetStyle().contentMargin;
      pData->outlineWidth = GetStyle().outlineWidth;

      m_pState = new StaticState(pData);
    }

    Container::~Container()
    {
      m_pState->Destroy();
      delete m_pState;
    }

    Container * Container::Create(Widget * a_pParent, vec2 const a_position, vec2 const & a_size, std::initializer_list<WidgetFlag> a_flags)
    {
      return new Container(a_pParent, a_position, a_size, a_flags);
    }

    void Container::_HandleMessage(Message * a_pMsg)
    {
      UpdateState(m_pState->HandleMessage(a_pMsg));
    }

    void Container::Clear()
    {
      m_pState->Clear();
    }

    void Container::Add(Widget * a_pMsg)
    {
      a_pMsg->SetParent(this);
      m_pState->Add(a_pMsg);
    }

    void Container::Remove(Widget * a_pMsg)
    {
      m_pState->Remove(a_pMsg);
    }

    void Container::Draw()
    {
      m_pState->Draw();
    }

    WidgetState Container::QueryState() const
    {
      return m_pState->QueryState();
    }

    Widget * Container::GetParent() const
    {
      return m_pState->GetParent();
    }

    void Container::SetParent(Widget * a_pParent)
    {
      m_pState->SetParent(a_pParent);
    }

    void Container::UpdateState(InternalState * a_pState)
    {
      if (a_pState != nullptr)
      {
        delete m_pState;
        m_pState = a_pState;
      }
    }

    void Container::_SetLocalPosition(vec2 const & a_pos)
    {
      m_pState->_SetLocalPosition(a_pos);
    }

    void Container::_SetSize(vec2 const & a_size)
    {
      m_pState->_SetSize(a_size);
    }

    vec2 Container::_GetLocalPosition()
    {
      return m_pState->_GetLocalPosition();
    }

    vec2 Container::_GetSize()
    {
      return m_pState->_GetSize();
    }

    void Container::SetColour(ContainerElement a_ele, Colour a_clr)
    {
      m_pState->SetColour(a_ele, a_clr);
    }

    bool Container::IsContainer() const
    {
      return true;
    }

    vec2 Container::GetContentDivPosition()
    {
      return m_pState->GetContentDivPosition();
    }

    vec2 Container::GetContentDivSize()
    {
      return m_pState->GetContentDivSize();
    }

    void Container::SetContentMargin(float a_size)
    {
      m_pState->SetContentMargin(a_size);
    }
  }
}