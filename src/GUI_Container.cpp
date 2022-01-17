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

      // Default size is needed for containers where all content is stretch
      void FitSizeToContent(vec2 const& defaultSize);
      UIAABB GetContentAABB(vec2 const& defaultSize) const;
      void SetAABB(UIAABB const& newAABB);

      Widget *pParent;
      Widget *pFocus;
      UIAABB aabb;
      WidgetState state;
      Dg::DoublyLinkedList<Widget *> children;
    };

    //------------------------------------------------------------------------------------
    // PIMPL
    //------------------------------------------------------------------------------------

    UIAABB Container::PIMPL::GetContentAABB(vec2 const& defaultSize) const
    {
      int const x = 0;
      int const y = 1;
      bool hasStretch[2] = { false, false };
      vec2 minPoint(FLT_MAX, FLT_MAX);
      vec2 maxPoint(-FLT_MAX, -FLT_MAX);
      UIAABB result = {};

      for (auto it = children.cbegin(); it != children.cend(); it++)
      {
        for (int i = 0; i < 2; i++)
        {
          if ((*it)->HasFlag(WidgetFlag::StretchWidth))
          {
            hasStretch[i] = true;
          }
          else
          {
            float s = (*it)->GetSize()[i];
            float p = (*it)->GetLocalPosition()[i];

            if (p < minPoint[i])
              minPoint[i] = p;
            if (p + s > maxPoint[i])
              maxPoint[i] = p + s;
          }
        }
      }

      for (int i = 0; i < 2; i++)
      {
        if (minPoint[i] == FLT_MAX)
        {
          result.position[i] = 0.0f;
          if (hasStretch[i])
            result.size[i] = defaultSize[i];
          else
            result.size[i] = 0.0f;
        }
        else
        {
          result.position[i] = minPoint[i];
          result.size[i] = maxPoint[i] - minPoint[i];
        }
      }

      return result;
    }

    void Container::PIMPL::FitSizeToContent(vec2 const &defaultSize)
    {
      UIAABB contentAABB = GetContentAABB(defaultSize);

      vec2 offset(0.0f, 0.0f);
      if (contentAABB.position.x() < 0.0f)
        offset.x() = -contentAABB.position.x();

      if (contentAABB.position.y() < 0.0f)
        offset.y() = -contentAABB.position.y();

      aabb.size = offset + contentAABB.position + contentAABB.size;

      // Move offset to origin if negative
      for (auto it = children.begin(); it != children.end(); it++)
      {
        (*it)->SetLocalPosition((*it)->GetLocalPosition() + offset);
        (*it)->UpdateSize();
      }
    }

    void Container::PIMPL::SetAABB(UIAABB const& newAABB)
    {
      aabb = newAABB;

      // Move offset to origin if negative
      for (auto it = children.begin(); it != children.end(); it++)
        (*it)->UpdateSize();
    }

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

      // We pass messages to other containers last.
      for (auto it = children.end(); it != children.begin();)
      {
        it--;
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
      for (auto it = children.end(); it != children.begin();)
      {
        it--;
        (*it)->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          state = (*it)->QueryState();
          if ((*it)->QueryState() == WidgetState::HasFocus)
            pFocus = (*it);
          break;
        }
      }
    }

    void Container::PIMPL::HandleMessage(Message_GUI_PointerMove *a_pMsg, Container *pContainer)
    {
      vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

      for (auto it = children.end(); it != children.begin();)
      {
        it--;
        (*it)->HandleMessage(a_pMsg);
        if (a_pMsg->QueryFlag(DgE::Message::Flag::Handled))
        {
          state = (*it)->QueryState();
          if ((*it)->QueryState() == WidgetState::HasFocus)
            pFocus = (*it);
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

      for (auto it = m_pimpl->children.begin(); it != m_pimpl->children.end(); it++)
      {
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

    bool Container::Empty() const
    {
      return m_pimpl->children.size() == 0;
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

    void Container::Move(Widget* a_pWgt, vec2 const& a_pos)
    {
      for (auto it = m_pimpl->children.begin(); it != m_pimpl->children.end(); it++)
      {
        if (*it == a_pWgt)
        {
          (*it)->SetLocalPosition(a_pos);
          break;
        }
      }
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

    void Container::FitSizeToContent(vec2 const& defaultSize)
    {
      m_pimpl->FitSizeToContent(defaultSize);
    }

    void Container::SetLocalPosition(vec2 const &position)
    {
      m_pimpl->aabb.position = position;
    }

    void Container::SetSize(vec2 const &size)
    {
      m_pimpl->aabb.size = size;
    }

    vec2 Container::GetLocalPosition()
    {
      return m_pimpl->aabb.position;
    }

    vec2 Container::GetSize()
    {
      return m_pimpl->aabb.size;
    }

  }
}