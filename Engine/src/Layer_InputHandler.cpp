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
  // Layer_InputHandler
  //------------------------------------------------------------------------------------

  Layer_InputHandler::Layer_InputHandler()
    : m_pEventPoller(Framework::Instance()->GetEventPoller())
  {

  }

  Layer_InputHandler::~Layer_InputHandler()
  {

  }

  void Layer_InputHandler::HandleMessage(Message *)
  {
  
  }

  void Layer_InputHandler::ClearBindings()
  {
    m_bindings.clear();
  }

  void Layer_InputHandler::AddBinding(uint32_t a_inputMessageID, InputMessageTranslator a_callback)
  {
    m_bindings[a_inputMessageID] = a_callback;
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
      TRef<Message> pMsg = m_pEventPoller->NextEvent();
      if (pMsg.Get() == nullptr)
        break;

      auto it = m_bindings.find(pMsg->GetID());
      if (it != m_bindings.end())
        it->second(pMsg);
      else
        POST(pMsg); // TODO Remove this! Event message should not leave this scope!
    }
  }
}
