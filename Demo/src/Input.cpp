//
//#include "Input.h"
//#include "Message.h"
//#include "Application.h"
//#include "System_Window.h"
//
//struct InputData
//{
//  vec2 screenDim;
//};
//
//class InputState
//{
//public:
//
//  InputState(InputData & a_data)
//    : m_data(a_data)
//  { }
//
//  virtual void HandleMessage(Message * a_pMsg) = 0;
//  virtual void OnAttach() = 0;
//  virtual void OnDetach() = 0;
//
//private:
//
//  InputData & m_data;
//};
//
//class InputState_GUI : public InputState
//{
//public:
//
//  InputState_GUI(InputData & a_data)
//    : InputState(a_data)
//  {}
//
//  void HandleMessage(Message * a_pMsg) override;
//  void OnAttach() override;
//  void OnDetach() override;
//};
//
//class InputState_Camera : public InputState
//{
//public:
//
//  InputState_Camera(InputData & a_data)
//    : InputState(a_data)
//  {}
//
//  void HandleMessage(Message * a_pMsg) override;
//  void OnAttach() override;
//  void OnDetach() override;
//};
//
//class Input::PIMPL
//{
//public:
//
//  PIMPL()
//    : pState(nullptr)
//    , data{}
//  {  }
//
//  ~PIMPL()
//  {
//    delete pState;
//  }
//
//  InputState *pState;
//  InputData data;
//};
//
//Input::Input()
//  : m_pimpl(new PIMPL())
//{
//  DgE::System_Window * pWindow = GET_SYSTEM(DgE::System_Window);
//  int w = 0, h = 0;
//  if (pWindow == nullptr)
//  {
//    LOG_ERROR("Input::Input() -> Failed to find Window System");
//    m_pimpl->data.screenDim.x() = 100.f;
//    m_pimpl->data.screenDim.y() = 100.f;
//  }
//  else
//  {
//    pWindow->GetDimensions(w, h);
//    m_pimpl->data.screenDim.x() = (float)w;
//    m_pimpl->data.screenDim.y() = (float)h;
//  }
//
//  m_pimpl->pState = new InputState_GUI(m_pimpl->data);
//}
//
//Input::~Input()
//{
//  delete m_pimpl;
//}
//
//void Input::OnAttach()
//{
//  m_pimpl->pState->OnAttach();
//}
//
//void Input::OnDetach()
//{
//  m_pimpl->pState->OnDetach();
//}
//
//void Input::HandleMessage(Message * a_pMsg)
//{
//  DISPATCH_MESSAGE(Message_Window_Resized);
//  m_pimpl->pState->HandleMessage(a_pMsg);
//}
//
//void Input::HandleMessage(Message_Window_Resized * a_pMsg)
//{
//  m_pimpl->data.screenDim.x() = (float)a_pMsg->w;
//  m_pimpl->data.screenDim.y() = (float)a_pMsg->h;
//}