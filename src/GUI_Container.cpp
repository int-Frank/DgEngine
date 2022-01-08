//@group GUI

#include "DgDoublyLinkedList.h"

#include "GUI_Container.h"
#include "GUI_Internal.h"

namespace DgE
{
  namespace GUI
  {
    //------------------------------------------------------------------------------------
    // PIMPL
    //------------------------------------------------------------------------------------

    class Container::PIMPL
    {
    public:

      PIMPL()
        : pParent(nullptr)
        , pFocus(nullptr)
        , aabb{}
        , state(WidgetState::None)
        , children()
      {

      }

      ~PIMPL()
      {
        Clear();
      }

      void Clear()
      {
        for (auto pWgt : children)
          delete pWgt;
        children.clear();
      }

      void HandleMessage(Message_GUI_PointerUp *, Container *pContainer);
      void HandleMessage(Message_GUI_PointerDown *, Container *pContainer);
      void HandleMessage(Message_GUI_PointerMove *, Container *pContainer);

      Widget *pParent;
      Widget *pFocus;
      UIAABB aabb;
      WidgetState state;
      Dg::DoublyLinkedList<Widget *> children;
    };

    //------------------------------------------------------------------------------------
    // Contaier
    //------------------------------------------------------------------------------------


    Container::Container(vec2 const position, vec2 const &size)
      : Widget({}, {})
      , m_pimpl(new PIMPL())
    {
      m_pimpl->aabb = {position, size};
    }

    Container *Container::Create(vec2 const position, vec2 const &size)
    {
      return new Container(position, size);
    }

    Container::~Container()
    {
      delete m_pimpl;
    }

    void Container::_HandleMessage(Message *pMsg)
    {
      if (pMsg->GetCategory() != MC_GUI)
        return;

      if (m_pimpl->pFocus != nullptr)
      {
        m_pimpl->pFocus->HandleMessage(pMsg);
        m_pimpl->state = m_pimpl->pFocus->QueryState();
        if (m_pimpl->state == WidgetState::None)
          m_pimpl->pFocus = nullptr;
      }

      if (pMsg->QueryFlag(Message::Flag::Handled))
        return;

      if (pMsg->GetID() == Message_GUI_PointerDown::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerDown *>(pMsg), this);
      else if (pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerUp *>(pMsg), this);
      else if (pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
        m_pimpl->HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(pMsg), this);
    }

   void Container::PIMPL::HandleMessage(Message_GUI_PointerDown *a_pMsg, Container *pContainer)
    {
      UIAABB aabb;
      if (!pContainer->GetGlobalViewableArea(aabb))
        return;

      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      if (!PointInBox(point, aabb))
        return;

      for (auto it = children.begin(); it != children.end(); it++)
      {
        (*it)->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          state = (*it)->QueryState();
          if ((*it)->QueryState() == WidgetState::HasFocus)
            pFocus = (*it);

          if ((*it)->IsContainer())
          {
            Widget *pWgt = (*it);
            children.erase(it);
            children.push_front(pWgt); // We do this to render the container on top
          }
          break;
        }
      }
    }

    void Container::PIMPL::HandleMessage(Message_GUI_PointerUp *a_pMsg, Container *pContainer)
    {
      for (Widget *pWidget : children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          state = pWidget->QueryState();
          if (pWidget->QueryState() == WidgetState::HasFocus)
            pFocus = pWidget;
          break;
        }
      }
    }

    void Container::PIMPL::HandleMessage(Message_GUI_PointerMove *a_pMsg, Container *pContainer)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      for (Widget *pWidget : children)
      {
        pWidget->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          state = pWidget->QueryState();
          if (pWidget->QueryState() == WidgetState::HasFocus)
            pFocus = pWidget;
          break;
        }
      }

      UIAABB aabb;
      if (!pContainer->GetGlobalViewableArea(aabb))
      {
        vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

        if (PointInBox(point, aabb))
          a_pMsg->ConsumeHover();
      }
    }

    void Container::Draw()
    {
      UIAABB viewableWindow;

      if (!GetGlobalViewableArea(viewableWindow))
        return;

      for (auto it = m_pimpl->children.end(); it != m_pimpl->children.begin();)
      {
        it--;
        (*it)->Draw();
      }
    }

    Widget *Container::GetParent() const
    {
      return m_pimpl->pParent;
    }

    void Container::SetParent(Widget *pParent)
    {
      m_pimpl->pParent = pParent;
    }

    WidgetState Container::QueryState() const
    {
      return m_pimpl->state;
    }

    void Container::Clear()
    {
      m_pimpl->Clear();
    }

    void Container::Add(Widget *a_pWgt)
    {
      a_pWgt->SetParent(this);
      if (a_pWgt->IsContainer())
        m_pimpl->children.push_front(a_pWgt);
      else
        m_pimpl->children.push_back(a_pWgt);
    }

    void Container::Remove(Widget *a_pWgt)
    {
      for (auto it = m_pimpl->children.begin(); it != m_pimpl->children.end(); it++)
      {
        if (*it == a_pWgt)
        {
          delete *it;
          m_pimpl->children.erase(it);
          break;
        }
      }
    }

    bool Container::IsContainer() const
    {
      return true;
    }

    void Container::_SetLocalPosition(vec2 const &position)
    {
      m_pimpl->aabb.position = position;
    }

    void Container::_SetSize(vec2 const &size)
    {
      m_pimpl->aabb.size = size;
    }

    vec2 Container::_GetLocalPosition()
    {
      return m_pimpl->aabb.position;
    }

    vec2 Container::_GetSize()
    {
      return m_pimpl->aabb.size;
    }

  }
}