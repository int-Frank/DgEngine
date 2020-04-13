//@group Layers

#include <cstring>

#include "Layer_InputHandler.h"
#include "Framework.h"
#include "core_Log.h"
#include "InputCodes.h"
#include "EngineMessages.h"
#include "MessageBus.h"

#include "IEventPoller.h"
#include "IMouseController.h"

namespace Engine
{
  //------------------------------------------------------------------------------------
  // InputBinding
  //------------------------------------------------------------------------------------

  InputBinding::InputBinding(InputCode a_code, InputEvent a_event, Message* a_pMsg)
    : code(a_code)
    , evnt(a_event)
    , pMsg(a_pMsg)
  {

  }

  InputBinding::InputBinding(InputCode a_code, Message* a_pMsg)
    : code(a_code)
    , evnt(IE_NA)
    , pMsg(a_pMsg)
  {

  }

  //------------------------------------------------------------------------------------
  // Layer_InputHandler
  //------------------------------------------------------------------------------------

  Layer_InputHandler::Layer_InputHandler()
    : m_eventPoller(Framework::Instance()->GetEventPoller())
    , m_mouseController(Framework::Instance()->GetMouseController())
    , m_xMouseRotRate(1.0f)
    , m_yMouseRotRate(1.0f)
  {

  }

  Layer_InputHandler::~Layer_InputHandler()
  {

  }

  void Layer_InputHandler::HandleBinding(BindingKey a_key, Message const * a_pMsg)
  {
    auto it = m_bindings.find(a_key);
    if (it != m_bindings.end())
    {
      TRef<Message> pMsg = it->second->CloneAsTRef();
      MessageTranslator::Translate(pMsg.Get(), a_pMsg);
      POST(pMsg);
    }
  }

  void Layer_InputHandler::HandleMessage(Message* a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_Input)
      return;

    DISPATCH_MESSAGE(Message_Input_Text);
    DISPATCH_MESSAGE(Message_Input_KeyUp);
    DISPATCH_MESSAGE(Message_Input_KeyDown);
    DISPATCH_MESSAGE(Message_Input_MouseButtonUp);
    DISPATCH_MESSAGE(Message_Input_MouseButtonDown);
    DISPATCH_MESSAGE(Message_Input_MouseWheelUp);
    DISPATCH_MESSAGE(Message_Input_MouseWheelDown);
    DISPATCH_MESSAGE(Message_Input_MouseMove);
  }

  void Layer_InputHandler::HandleMessage(Message_Input_Text* a_pMsg)
  {
    BindingKey mapKey = PackKey(IC_TEXT, IE_NA);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_KeyUp * a_pMsg)
  {
    BindingKey mapKey = PackKey(InputCode(a_pMsg->keyCode), IE_BUTTON_UP);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_KeyDown * a_pMsg)
  {
    BindingKey mapKey = PackKey(InputCode(a_pMsg->keyCode), IE_BUTTON_DOWN);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_MouseButtonUp * a_pMsg)
  {
    BindingKey mapKey = PackKey(InputCode(a_pMsg->button), IE_BUTTON_UP);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_MouseButtonDown * a_pMsg)
  {
    BindingKey mapKey = PackKey(InputCode(a_pMsg->button), IE_BUTTON_DOWN);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_MouseWheelUp * a_pMsg)
  {
    BindingKey mapKey = PackKey(IC_MOUSE_WHEEL_UP, IE_NA);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_MouseWheelDown * a_pMsg)
  {
    BindingKey mapKey = PackKey(IC_MOUSE_WHEEL_DOWN, IE_NA);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void  Layer_InputHandler::HandleMessage(Message_Input_MouseMove * a_pMsg)
  {
    BindingKey mapKey = PackKey(IC_MOUSE_MOTION, IE_NA);
    HandleBinding(mapKey, a_pMsg);
    a_pMsg->SetFlag(Message::Flag::Handled);
  }

  void Layer_InputHandler::GrabMouse()
  {
    m_mouseController->Grab();
  }

  void Layer_InputHandler::ReleaseMouse()
  {
    m_mouseController->Release();
  }

  void Layer_InputHandler::SetMouseLookRate(float a_xRate, float a_yRate)
  {
    m_xMouseRotRate = a_xRate;
    m_yMouseRotRate = a_yRate;
  }

  Layer_InputHandler::BindingKey Layer_InputHandler::PackKey(InputCode a_code, InputEvent a_event)
  {
    return (a_code << 16) | a_event;
  }

  void Layer_InputHandler::ClearBindings()
  {
    m_bindings.clear();
  }

  void Layer_InputHandler::SetBindings(std::initializer_list<InputBinding> const & a_bindings)
  {
    for (auto const & item : a_bindings)
      m_bindings.insert(PackKey(item.code, item.evnt), item.pMsg);
  }

  /*template<>
  void Layer_InputHandler::_SetProfile<Layer_InputHandler::BP_Menu>()
  {
    m_bindings.clear();

    Bind(IC_MOUSE_MOTION, Message_Input_OtherMouseEvent, Message_GUI_MouseMove);
    Bind(IC_MOUSE_WHEEL_UP, Message_Input_OtherMouseEvent, Message_GUI_MouseWheelUp);
    Bind(IC_MOUSE_WHEEL_DOWN, Message_Input_OtherMouseEvent, Message_GUI_MouseWheelDown);
    Bind(IC_MOUSE_BUTTON_LEFT, Message_Input_ButtonDown, Message_GUI_MouseButtonDown);
    Bind(IC_MOUSE_BUTTON_LEFT, Message_Input_ButtonUp, Message_GUI_MouseButtonUp);

    Bind(IC_UNKNOWN, Message_Input_Text, Message_GUI_Text);

    BindKeyDown(IC_KEY_TAB, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_LEFT, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_RIGHT, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_UP, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_DOWN, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_PAGEUP, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_PAGEDOWN, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_HOME, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_END, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_INSERT, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_DELETE, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_BACKSPACE, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_SPACE, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_ENTER, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_KPENTER, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_A, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_C, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_V, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_X, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_Y, true, Message_GUI_KeyDown);
    BindKeyDown(IC_KEY_Z, true, Message_GUI_KeyDown);

    Bind(IC_KEY_TAB, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_LEFT, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_RIGHT, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_UP, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_DOWN, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_PAGEUP, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_PAGEDOWN, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_HOME, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_END, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_INSERT, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_DELETE, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_BACKSPACE, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_SPACE, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_ENTER, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_KPENTER, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_A, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_C, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_V, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_X, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_Y, Message_Input_KeyUp, Message_GUI_KeyUp);
    Bind(IC_KEY_Z, Message_Input_KeyUp, Message_GUI_KeyUp);

    Bind(IC_KEY_ESC, Message_Input_KeyDown, Message_GoBack);

    m_mouseController->Release();
  }*/

  void Layer_InputHandler::Update(float a_dt)
  {
    while (true)
    {
      TRef<Message> pMsg = m_eventPoller->NextEvent();
      if (pMsg.Get() == nullptr)
        break;

      HandleMessage(&*pMsg);
      if (!pMsg->Is(Message::Flag::Handled))
        POST(pMsg);
    }
  }
}
