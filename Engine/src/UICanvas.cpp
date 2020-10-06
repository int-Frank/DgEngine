//@group UI
//
//#include "UICanvas.h"
//#include "UICommon.h"
//#include "UIWidget.h"
//
//namespace Engine
//{
//  class UICanvas::CanvasState
//  {
//  public:
//
//    CanvasState(Dg::DoublyLinkedList<UIWidget *> const & widgets)
//      : m_widgets(widgets)
//    {
//    }
//
//    virtual void Init() = 0;
//    virtual void Deinit() = 0;
//
//    virtual UICanvas::CanvasState * DoMouseMove(vec2 const &)  {return nullptr;}
//    virtual UICanvas::CanvasState * DoMouseDown(vec2 const &)  {return nullptr;}
//    virtual UICanvas::CanvasState * DoMouseUp(vec2 const &)    {return nullptr;}
//    virtual UICanvas::CanvasState * DoScroll(float x)          {return nullptr;}
//    virtual UICanvas::CanvasState * DoKeyDown(InputCode)       {return nullptr;}
//    virtual UICanvas::CanvasState * DoKeyUp(InputCode)         {return nullptr;}
//    virtual UICanvas::CanvasState * DoText(char text[32])      {return nullptr;}
//    virtual UICanvas::CanvasState * DoActivate()               {return nullptr;}
//
//  protected:
//
//    UIWidget * FindFocus(vec2 const &);
//
//  protected:
//    Dg::DoublyLinkedList<UIWidget *> const &m_widgets;
//  };
//
//  class UIState_Idle : public UICanvas::CanvasState
//  {
//  public:
//
//    UIState_Idle(Dg::DoublyLinkedList<UIWidget *> const & widgets, UIWidget *pHover = nullptr);
//
//    void Init() override;
//    void Deinit() override;
//
//    UICanvas::CanvasState * DoMouseMove(vec2 const &) override;
//    UICanvas::CanvasState * DoMouseDown(vec2 const &) override;
//    UICanvas::CanvasState * DoScroll(float x) override;
//    UICanvas::CanvasState * DoActivate() override;
//
//  private:
//
//    UIWidget * FindFocus(vec2 const &);
//
//  private:
//
//    UIWidget * m_pHover;
//  };
//
//  class UIState_Focus : public UICanvas::CanvasState
//  {
//  public:
//
//    UIState_Focus(Dg::DoublyLinkedList<UIWidget *> const & widgets, UIWidget *pFocus);
//
//    void Init() override;
//    void Deinit() override;
//
//    UICanvas::CanvasState * DoMouseDown(vec2 const &) override;
//    UICanvas::CanvasState * DoScroll(float x) override;
//    UICanvas::CanvasState * DoKeyDown(InputCode) override;
//    UICanvas::CanvasState * DoKeyUp(InputCode) override;
//    UICanvas::CanvasState * DoText(char text[32]) override;
//    UICanvas::CanvasState * DoActivate() override;
//
//  private:
//    UIWidget *m_pFocus;
//  };
//
//  //--------------------------------------------------------------------------------
//  // UIState_Idle
//  //--------------------------------------------------------------------------------
//  
//  UIState_Idle::UIState_Idle(Dg::DoublyLinkedList<UIWidget *> const & widgets, UIWidget *pHover)
//    : CanvasState(widgets)
//    , m_pHover(pHover)
//  {
//
//  }
//
//  void UIState_Idle::Init()
//  {
//    if (m_pHover != nullptr)
//      m_pHover->HoverOn();
//  }
//
//  void UIState_Idle::Deinit()
//  {
//    if (m_pHover != nullptr)
//      m_pHover->HoverOff();
//  }
//
//  UIWidget * UIState_Idle::FindFocus(vec2 const & xy)
//  {
//    UIWidget * pFocus = nullptr;
//
//    for (auto it = m_widgets.cbegin(); it != m_widgets.cend(); it++)
//    {
//      if ((*it)->IsInside(xy))
//      {
//        if (pFocus == nullptr)
//        {
//          pFocus = *it;
//        }
//        else
//        {
//          if ((*it)->GetDepth() < pFocus->GetDepth())
//            pFocus = (*it);
//        }
//      }
//    }
//    return pFocus;
//  }
//
//  UICanvas::CanvasState * UIState_Idle::DoMouseMove(vec2 const & xy)
//  {
//    UIWidget * pHover = FindFocus(xy);
//    if (pHover != m_pHover)
//    {
//      if (m_pHover != nullptr)
//        m_pHover->HoverOff();
//      if (pHover != nullptr)
//        m_pHover->HoverOn();
//      m_pHover = pHover;
//    }
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Idle::DoMouseDown(vec2 const & xy)
//  {
//    m_pHover = FindFocus(xy);
//    if (m_pHover == nullptr)
//      return nullptr;
//
//    if (m_pHover->DoMouseDown() == UIState::Active)
//      return new UIState_Focus(m_widgets, m_pHover);
//
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Idle::DoScroll(float x)
//  {
//    if (m_pHover == nullptr)
//      return nullptr;
//
//    if (m_pHover->DoScroll(x) == UIState::Active)
//      return new UIState_Focus(m_widgets, m_pHover);
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Idle::DoActivate()
//  {
//    if (m_pHover == nullptr)
//      return nullptr;
//
//    if (m_pHover->Activate() == UIState::Active)
//      return new UIState_Focus(m_widgets, m_pHover);
//    return nullptr;
//  }
//
//  //--------------------------------------------------------------------------------
//  // UIState_Focus
//  //--------------------------------------------------------------------------------
//
//  UIState_Focus::UIState_Focus(Dg::DoublyLinkedList<UIWidget *> const & widgets, UIWidget * pFocus)
//    : CanvasState(widgets)
//    , m_pFocus(pFocus)
//  {
//
//  }
//
//  void UIState_Focus::Init()
//  {
//    if (m_pFocus != nullptr)
//      m_pFocus->FocusOn();
//  }
//
//  void UIState_Focus::Deinit()
//  {
//    if (m_pFocus != nullptr)
//      m_pFocus->FocusOff();
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoMouseDown(vec2 const & xy)
//  {
//    if (m_pFocus->IsInside(xy))
//    {
//      if (m_pFocus->DoMouseDown() != UIState::Active)
//        return new UIState_Idle(m_widgets, m_pFocus);
//      return nullptr;
//    }
//
//    return new UIState_Idle(m_widgets, FindFocus(xy));
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoScroll(float x)
//  {
//    if (m_pFocus->DoScroll(x) != UIState::Active)
//      return new UIState_Idle(m_widgets, m_pFocus);
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoKeyDown(InputCode code)
//  {
//    if (m_pFocus->DoKeyDown(code) != UIState::Active)
//      return new UIState_Idle(m_widgets, m_pFocus);
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoKeyUp(InputCode code)
//  {
//    if (m_pFocus->DoKeyUp(code) != UIState::Active)
//      return new UIState_Idle(m_widgets, m_pFocus);
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoText(char text[32])
//  {
//    if (m_pFocus->DoText(text) != UIState::Active)
//      return new UIState_Idle(m_widgets, m_pFocus);
//    return nullptr;
//  }
//
//  UICanvas::CanvasState * UIState_Focus::DoActivate()
//  {
//    if (m_pFocus->Activate() != UIState::Active)
//      return new UIState_Idle(m_widgets, m_pFocus);
//    return nullptr;
//  }
//
//  //--------------------------------------------------------------------------------
//  // UICanvas
//  //--------------------------------------------------------------------------------
//
//  UICanvas::UICanvas()
//    : m_widgets()
//    , m_pState(new UIState_Idle(m_widgets))
//  {
//    m_pState->Init();
//  }
//
//  UICanvas::~UICanvas()
//  {
//    m_pState->Deinit();
//    delete m_pState;
//    Clear();
//  }
//
//  void UICanvas::Clear()
//  {
//    for (UIWidget * ptr : m_widgets)
//      delete ptr;
//
//    m_widgets.clear();
//    delete m_pState;
//    m_pState = new UIState_Idle(m_widgets);
//  }
//
//  void UICanvas::Add(UIWidget* a_pWgt)
//  {
//    bool inserted = false;
//    for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
//    {
//      if (a_pWgt->GetDepth() <= (*it)->GetDepth())
//      {
//        m_widgets.insert(it, a_pWgt);
//        inserted = true;
//      }
//    }
//
//    if (!inserted)
//      m_widgets.push_back(a_pWgt);
//  }
//
//  void UICanvas::ChangeState(CanvasState * pNewState)
//  {
//    if (pNewState != nullptr)
//    {
//      m_pState->Deinit();
//      delete m_pState;
//      pNewState->Init();
//      m_pState = pNewState;
//    }
//  }
//
//  void UICanvas::DoMouseMove(vec2 const & xy)
//  {
//    ChangeState(m_pState->DoMouseMove(xy));
//  }
//
//  void UICanvas::DoMouseDown(vec2 const & xy)
//  {
//    ChangeState(m_pState->DoMouseDown(xy));
//  }
//
//  void UICanvas::DoMouseUp(vec2 const & xy)
//  {
//    ChangeState(m_pState->DoMouseUp(xy));
//  }
//
//  void UICanvas::DoScroll(float x)
//  {
//    ChangeState(m_pState->DoScroll(x));
//  }
//
//  void UICanvas::DoKeyDown(InputCode code)
//  {
//    ChangeState(m_pState->DoKeyDown(code));
//  }
//
//  void UICanvas::DoKeyUp(InputCode code)
//  {
//    ChangeState(m_pState->DoKeyUp(code));
//  }
//
//  void UICanvas::DoText(char text[32])
//  {
//    ChangeState(m_pState->DoText(text));
//  }
//
//  void UICanvas::DoActivate()
//  {
//    ChangeState(m_pState->DoActivate());
//  }
//
//  void UICanvas::Draw()
//  {
//    // Bind UI texture
//
//    for (UIWidget * pWidget : m_widgets)
//      pWidget->Draw();
//
//    //Unbind UI texture
//  }
//}