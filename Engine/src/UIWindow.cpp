//@group UI

#include "UIWindow.h"
#include "UIWidget.h"
#include "UICommon.h"
#include "UI_Internal.h"

namespace Engine
{
  class WindowGrab
  {
  public:

    enum Type
    {
      Square,
      Rounded,

      COUNT
    };

    WindowGrab(Type = Square);

    vec2 Size() const;
    bool Intersects(vec2 const & point) const;
    void Draw(vec2 const & position, Colour clr);

  private:

    static uint16_t s_masks[COUNT][16];

    uint16_t m_mask[16];
  };

  //------------------------------------------------------------------------------------
  // State class declarations
  //------------------------------------------------------------------------------------
  class UIWindowState
  {
  public:

    struct Data
    {
      Colour clrBackground;
      Colour clrGrab;
      Colour clrGrabActive;
      Colour clrGrabCurrent;

      UIWidget * pParent;
      vec2 position;
      vec2 size;
      WindowGrab grab;
      Dg::DoublyLinkedList<UIWidget *> children;
      uint32_t flags;
    };

    UIWindowState(Data * a_pData);
    virtual ~UIWindowState();

    void Destroy();

    virtual UIState QueryState() const = 0;
    UIWidget * GetParent() const;
    void SetParent(UIWidget * a_pParent);

    vec2 GetPosition() const;
    vec2 GetSize() const;
    void Clear();

    void Add(UIWidget * a_pWgt);
    void Remove(UIWidget * a_pWgt);

    void Draw();

    virtual UIWindowState * HandleMessage(Message *) = 0;

  protected:

    bool HasFlag(UIWindow::Flag) const;

    Data * m_pData;
  };

  class StaticState : public UIWindowState
  {
  public:

    StaticState(Data * a_pData);
    ~StaticState();

    UIState QueryState() const override;

    UIWindowState * HandleMessage(Message * a_pMsg) override;
    UIWindowState * HandleMessage(Message_GUI_PointerDown * a_pMsg);
    UIWindowState * HandleMessage(Message_GUI_PointerUp * a_pMsg);
    UIWindowState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

  private:

    UIWidget * m_pFocus;
    UIState m_state;
  };

  class MoveState : public UIWindowState
  {
  public:

    MoveState(Data * a_pData, vec2 const & a_controlAnchor);
    ~MoveState();

    UIState QueryState() const override;

    UIWindowState * HandleMessage(Message * a_pMsg) override;
    UIWindowState * HandleMessage(Message_GUI_PointerMove * a_pMsg);

  private:

    vec2 m_controlAnchor;
    vec2 m_positionAnchor;
  };

  class ResizeState : public UIWindowState
  {
  public:

    ResizeState(Data * a_pData, vec2 const & a_controlAnchor);
    ~ResizeState();
  
    UIState QueryState() const override;
  
    UIWindowState * HandleMessage(Message *) override;
    UIWindowState * HandleMessage(Message_GUI_PointerMove *);

  private:

    vec2 m_controlAnchor;
    vec2 m_sizeAnchor;
  };

  //------------------------------------------------------------------------------------
  // WindowGrab
  //------------------------------------------------------------------------------------

  uint16_t WindowGrab::s_masks[WindowGrab::COUNT][16] =
  {
    {
      0x1, 0x3, 0x7, 0xF,
      0x1F, 0x3F, 0x7F, 0xFF,
      0x1FF, 0x3FF, 0x7FF, 0xFFF,
      0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
    },
    {
      0x1, 0x3, 0x7, 0xF,
      0x1F, 0x3F, 0x7F, 0xFF,
      0x1FF, 0x3FF, 0x7FF, 0xFFF,
      0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
    }
  };

  bool WindowGrab::Intersects(vec2 const & point) const
  {
    int y = (int)point.y();
    if (y < 0 || y > 15)
      return false;

    int x = (int)point.x();
    if (x < 0 || x > 15)
      return false;

    return ((1 << (15 - x)) & m_mask[y]) != 0;
  }
  
  WindowGrab::WindowGrab(Type t)
    : m_mask{}
  {
    memcpy(m_mask, s_masks[t], 2 * 16);
  }

  vec2 WindowGrab::Size() const
  {
    return vec2(16.f, 16.f);
  }

  void WindowGrab::Draw(vec2 const & a_position, Colour a_clr)
  {
    UIRenderer::Instance()->DrawCorner(a_position, vec2(16.f, 16.f), a_clr);
  }

  //------------------------------------------------------------------------------------
  // UIWindowState
  //------------------------------------------------------------------------------------

  UIWindowState::UIWindowState(Data * a_pData)
    : m_pData(a_pData)
  {
    
  }

  UIWindowState::~UIWindowState()
  {
    
  }

  void UIWindowState::Destroy()
  {
    Clear();

    delete m_pData;
    m_pData = nullptr;
  }

  UIWidget * UIWindowState::GetParent() const
  {
    return m_pData->pParent;
  }

  void UIWindowState::SetParent(UIWidget * a_pParent)
  {
    m_pData->pParent = a_pParent;
  }

  vec2 UIWindowState::GetPosition() const
  {
    return m_pData->position;
  }

  vec2 UIWindowState::GetSize() const
  {
    return m_pData->size;
  }

  void UIWindowState::Clear()
  {
    for (auto pWgt : m_pData->children)
      delete pWgt;
    m_pData->children.clear();
  }

  void UIWindowState::Add(UIWidget * a_pWgt)
  {
    m_pData->children.push_back(a_pWgt);
  }

  void UIWindowState::Remove(UIWidget * a_pWgt)
  {
    for (auto it = m_pData->children.begin(); it != m_pData->children.end(); it++)
    {
      if (*it == a_pWgt)
      {
        delete *it;
        m_pData->children.erase(it);
        break;
      }
    }
  }

  void UIWindowState::Draw()
  {
    UIRenderer::Instance()->DrawBox(m_pData->position, m_pData->size, m_pData->clrBackground);
    if (HasFlag(UIWindow::Movable))
      m_pData->grab.Draw(m_pData->position + m_pData->size - m_pData->grab.Size(), m_pData->clrGrabCurrent);
  }

  bool UIWindowState::HasFlag(UIWindow::Flag a_flag) const
  {
    return (m_pData->flags & a_flag) != 0;
  }

  //------------------------------------------------------------------------------------
  // StaticState
  //------------------------------------------------------------------------------------

  StaticState::StaticState(Data * a_pData)
    : UIWindowState(a_pData)
    , m_pFocus(nullptr)
    , m_state(UIState::None)
  {
    
  }

  StaticState::~StaticState()
  {
    
  }

  UIState StaticState::QueryState() const
  {
    return m_state;
  }

  UIWindowState * StaticState::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return nullptr;

    UIWindowState * pResult = nullptr;

    if (m_pFocus != nullptr)
    {
      m_pFocus->HandleMessage(a_pMsg);
      if (m_pFocus->QueryState() == UIState::None)
      {
        m_pFocus = nullptr;
        m_state = UIState::None;
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

  UIWindowState * StaticState::HandleMessage(Message_GUI_PointerDown * a_pMsg)
  {
    for (UIWidget * pWidget : m_pData->children)
    {
      pWidget->HandleMessage(a_pMsg);
      if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
      {
        if (pWidget->QueryState() == UIState::HasFocus)
        {
          m_pFocus = pWidget;
          m_state = UIState::HasFocus;
        }
        return nullptr;
      }
    }
    vec2 point((float)a_pMsg->x, (float)a_pMsg->y);

    if (HasFlag(UIWindow::Resizable) && m_pData->grab.Intersects(point - (m_pData->position + m_pData->size - m_pData->grab.Size())))
      return new ResizeState(m_pData, point);

    if (HasFlag(UIWindow::Movable) && UIPointInBox(m_pData->position, m_pData->size, point))
      return new MoveState(m_pData, point);

    return nullptr;
  }

  UIWindowState * StaticState::HandleMessage(Message_GUI_PointerUp * a_pMsg)
  {
    for (UIWidget * pWidget : m_pData->children)
    {
      pWidget->HandleMessage(a_pMsg);
      if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
      {
        if (pWidget->QueryState() == UIState::HasFocus)
        {
          m_pFocus = pWidget;
          m_state = UIState::HasFocus;
        }
        break;
      }
    }
    return nullptr;
  }

  UIWindowState * StaticState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    for (UIWidget * pWidget : m_pData->children)
    {
      pWidget->HandleMessage(a_pMsg);
      if (a_pMsg->QueryFlag(Engine::Message::Flag::Handled))
      {
        if (pWidget->QueryState() == UIState::HasFocus)
        {
          m_pFocus = pWidget;
          m_state = UIState::HasFocus;
        }
        break;
      }
    }

    m_pData->clrGrabCurrent = m_pData->clrGrab;
    vec2 pointer((float)a_pMsg->x, (float)a_pMsg->y);
    if (m_pData->grab.Intersects(pointer - (m_pData->position + m_pData->size - m_pData->grab.Size())))
      m_pData->clrGrabCurrent = m_pData->clrGrabActive;

    return nullptr;
  }

  //------------------------------------------------------------------------------------
  // MoveState
  //------------------------------------------------------------------------------------

  MoveState::MoveState(Data * a_pData, vec2 const & a_controlAnchor)
    : UIWindowState(a_pData)
    , m_controlAnchor(a_controlAnchor)
    , m_positionAnchor(a_pData->position)
  {
    
  }

  MoveState::~MoveState()
  {
    
  }

  UIState MoveState::QueryState() const
  {
    return UIState::HasFocus;
  }

  UIWindowState * MoveState::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return nullptr;

    if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      return new StaticState(m_pData);

    if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
      return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

    return nullptr;
  }

  UIWindowState * MoveState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    vec2 point((float)a_pMsg->x, (float)a_pMsg->y);
    m_pData->position = m_positionAnchor + (point - m_controlAnchor);
    return nullptr;
  }

  //------------------------------------------------------------------------------------
  // ResizeState
  //------------------------------------------------------------------------------------

  ResizeState::ResizeState(Data * a_pData, vec2 const & a_controlAnchor)
    : UIWindowState(a_pData)
    , m_controlAnchor(a_controlAnchor)
    , m_sizeAnchor(a_pData->size)
  {

  }

  ResizeState::~ResizeState()
  {

  }

  UIState ResizeState::QueryState() const
  {
    return UIState::HasFocus;
  }

  UIWindowState * ResizeState::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_GUI)
      return nullptr;

    if (a_pMsg->GetID() == Message_GUI_PointerUp::GetStaticID())
      return new StaticState(m_pData);

    if (a_pMsg->GetID() == Message_GUI_PointerMove::GetStaticID())
      return HandleMessage(dynamic_cast<Message_GUI_PointerMove *>(a_pMsg));

    return nullptr;
  }

  UIWindowState * ResizeState::HandleMessage(Message_GUI_PointerMove * a_pMsg)
  {
    vec2 point((float)a_pMsg->x, (float)a_pMsg->y);
    vec2 newSize = m_sizeAnchor + (point - m_controlAnchor);

    if (newSize.x() < UIWindow::s_minSize.x())
      newSize.x() = UIWindow::s_minSize.x();

    if (newSize.y() < UIWindow::s_minSize.y())
      newSize.y() = UIWindow::s_minSize.y();

    m_pData->size = newSize;

    return nullptr;
  }

  //------------------------------------------------------------------------------------
  // UIWindow
  //------------------------------------------------------------------------------------

  vec2 const UIWindow::s_minSize = vec2(50.f, 20.f);

  UIWindow::UIWindow(UIWidget * a_pParent, vec2 const a_position, vec2 const & a_size, uint32_t a_flags)
    : m_pState(nullptr)
  {
    UIWindowState::Data * pData = new UIWindowState::Data();
    pData->clrBackground = 0xBB000000;
    pData->clrGrab = 0xFFFF0000;
    pData->clrGrabActive = 0xFFFF9999;
    pData->clrGrabCurrent = pData->clrGrab;
    pData->pParent = a_pParent;
    pData->position = a_position;
    pData->size = a_size;
    pData->flags = a_flags;
    m_pState = new StaticState(pData);
  }

  UIWindow::~UIWindow()
  {
    m_pState->Destroy();
    delete m_pState;
  }

  UIWindow * UIWindow::Create(UIWidget * a_pParent, vec2 const a_position, vec2 const & a_size, uint32_t a_flags)
  {
    return new UIWindow(a_pParent, a_position, a_size, a_flags);
  }

  void UIWindow::HandleMessage(Message * a_pMsg)
  {
    UpdateState(m_pState->HandleMessage(a_pMsg));
  }

  void UIWindow::Clear()
  {
    m_pState->Clear();
  }

  void UIWindow::Add(UIWidget * a_pMsg)
  {
    m_pState->Add(a_pMsg);
  }

  void UIWindow::Remove(UIWidget * a_pMsg)
  {
    m_pState->Remove(a_pMsg);
  }

  void UIWindow::Draw()
  {
    m_pState->Draw();
  }

  UIState UIWindow::QueryState() const
  {
    return m_pState->QueryState();
  }

  UIWidget * UIWindow::GetParent() const
  {
    return m_pState->GetParent();
  }
  
  void UIWindow::SetParent(UIWidget * a_pParent)
  {
    m_pState->SetParent(a_pParent);
  }

  void UIWindow::UpdateState(UIWindowState * a_pState)
  {
    if (a_pState != nullptr)
    {
      delete m_pState;
      m_pState = a_pState;
    }
  }
}