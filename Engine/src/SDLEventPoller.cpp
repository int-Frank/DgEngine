//@group Framework

#include <sstream>
#include <cstring>

#include "Framework.h"
#include "EngineMessages.h"
#include "InputCodes.h"
#include "Log.h"
#include "Memory.h"

#include "SDL_events.h"
#include "SDL.h"

#include "IEventPoller.h"


static_assert(TEXT_INPUT_TEXT_SIZE == SDL_TEXTINPUTEVENT_TEXT_SIZE, "text container incorrect size");

namespace Engine
{
  class FW_EventPoller : public IEventPoller
  {
  public:

    FW_EventPoller();
    ~FW_EventPoller();
    TRef<Message> NextEvent() override;

  private:

    uint16_t m_modState;
  };

  static uint16_t GetModState()
  {
    SDL_Keymod mods = SDL_GetModState();
    uint16_t result = 0;
    if ((mods & KMOD_LSHIFT) != 0)  result |= KM_LSHIFT;
    if ((mods & KMOD_LCTRL) != 0)   result |= KM_LCTRL;
    if ((mods & KMOD_LALT) != 0)    result |= KM_LALT;
    if ((mods & KMOD_LGUI) != 0)    result |= KM_LGUI;
    if ((mods & KMOD_RSHIFT) != 0)  result |= KM_RSHIFT;
    if ((mods & KMOD_RCTRL) != 0)   result |= KM_RCTRL;
    if ((mods & KMOD_RALT) != 0)    result |= KM_RALT;
    if ((mods & KMOD_RGUI) != 0)    result |= KM_RGUI;
    if ((mods & KMOD_NUM) != 0)     result |= KM_NUM;
    if ((mods & KMOD_CAPS) != 0)    result |= KM_CAPS;
    if ((mods & KMOD_MODE) != 0)    result |= KM_MODE;
    return result;
  }

  Dg::ErrorCode Framework::InitEventPoller()
  {
    SetEventPoller(new FW_EventPoller());
    return Dg::ErrorCode::None;
  }

  FW_EventPoller::FW_EventPoller()
    : m_modState(0)
  {
    SDL_Keymod modState = SDL_GetModState();
    if ((modState & KMOD_CAPS) != 0)
      m_modState |= KM_CAPS;
    if ((modState & KMOD_NUM) != 0)
      m_modState |= KM_NUM;
  }

  FW_EventPoller::~FW_EventPoller()
  {

  }

  static InputCode TranslateKeyCode(SDL_Scancode a_key)
  {
    switch (a_key)
    {
      case SDL_SCANCODE_UNKNOWN:
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_B:
      case SDL_SCANCODE_C:
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_E:
      case SDL_SCANCODE_F:
      case SDL_SCANCODE_G:
      case SDL_SCANCODE_H:
      case SDL_SCANCODE_I:
      case SDL_SCANCODE_J:
      case SDL_SCANCODE_K:
      case SDL_SCANCODE_L:
      case SDL_SCANCODE_M:
      case SDL_SCANCODE_N:
      case SDL_SCANCODE_O:
      case SDL_SCANCODE_P:
      case SDL_SCANCODE_Q:
      case SDL_SCANCODE_R:
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_T:
      case SDL_SCANCODE_U:
      case SDL_SCANCODE_V:
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_X:
      case SDL_SCANCODE_Y:
      case SDL_SCANCODE_Z:
      case SDL_SCANCODE_1:
      case SDL_SCANCODE_2:
      case SDL_SCANCODE_3:
      case SDL_SCANCODE_4:
      case SDL_SCANCODE_5:
      case SDL_SCANCODE_6:
      case SDL_SCANCODE_7:
      case SDL_SCANCODE_8:
      case SDL_SCANCODE_9:
      case SDL_SCANCODE_0:
      case SDL_SCANCODE_RETURN:
      case SDL_SCANCODE_ESCAPE:
      case SDL_SCANCODE_BACKSPACE:
      case SDL_SCANCODE_TAB:
      case SDL_SCANCODE_SPACE:
      case SDL_SCANCODE_MINUS:
      case SDL_SCANCODE_EQUALS:
      case SDL_SCANCODE_LEFTBRACKET:
      case SDL_SCANCODE_RIGHTBRACKET:
      case SDL_SCANCODE_BACKSLASH:
      case SDL_SCANCODE_NONUSHASH:
      case SDL_SCANCODE_SEMICOLON:
      case SDL_SCANCODE_APOSTROPHE:
      case SDL_SCANCODE_GRAVE:
      case SDL_SCANCODE_COMMA:
      case SDL_SCANCODE_PERIOD:
      case SDL_SCANCODE_SLASH:
      case SDL_SCANCODE_CAPSLOCK:
      case SDL_SCANCODE_F1:
      case SDL_SCANCODE_F2:
      case SDL_SCANCODE_F3:
      case SDL_SCANCODE_F4:
      case SDL_SCANCODE_F5:
      case SDL_SCANCODE_F6:
      case SDL_SCANCODE_F7:
      case SDL_SCANCODE_F8:
      case SDL_SCANCODE_F9:
      case SDL_SCANCODE_F10:
      case SDL_SCANCODE_F11:
      case SDL_SCANCODE_F12:
      case SDL_SCANCODE_PRINTSCREEN:
      case SDL_SCANCODE_SCROLLLOCK:
      case SDL_SCANCODE_PAUSE:
      case SDL_SCANCODE_INSERT:
      case SDL_SCANCODE_HOME:
      case SDL_SCANCODE_PAGEUP:
      case SDL_SCANCODE_DELETE:
      case SDL_SCANCODE_END:
      case SDL_SCANCODE_PAGEDOWN:
      case SDL_SCANCODE_RIGHT:
      case SDL_SCANCODE_LEFT:
      case SDL_SCANCODE_DOWN:
      case SDL_SCANCODE_UP:
      case SDL_SCANCODE_NUMLOCKCLEAR:
      case SDL_SCANCODE_KP_DIVIDE:
      case SDL_SCANCODE_KP_MULTIPLY:
      case SDL_SCANCODE_KP_MINUS:
      case SDL_SCANCODE_KP_PLUS:
      case SDL_SCANCODE_KP_ENTER:
      case SDL_SCANCODE_KP_1:
      case SDL_SCANCODE_KP_2:
      case SDL_SCANCODE_KP_3:
      case SDL_SCANCODE_KP_4:
      case SDL_SCANCODE_KP_5:
      case SDL_SCANCODE_KP_6:
      case SDL_SCANCODE_KP_7:
      case SDL_SCANCODE_KP_8:
      case SDL_SCANCODE_KP_9:
      case SDL_SCANCODE_KP_0:
      case SDL_SCANCODE_KP_PERIOD:
      case SDL_SCANCODE_NONUSBACKSLASH:
      case SDL_SCANCODE_APPLICATION:
      case SDL_SCANCODE_KP_EQUALS:
      case SDL_SCANCODE_F13:
      case SDL_SCANCODE_F14:
      case SDL_SCANCODE_F15:
      case SDL_SCANCODE_F16:
      case SDL_SCANCODE_F17:
      case SDL_SCANCODE_F18:
      case SDL_SCANCODE_F19:
      case SDL_SCANCODE_F20:
      case SDL_SCANCODE_F21:
      case SDL_SCANCODE_F22:
      case SDL_SCANCODE_F23:
      case SDL_SCANCODE_F24:
      case SDL_SCANCODE_KP_COMMA:
      case SDL_SCANCODE_KP_EQUALSAS400:
      case SDL_SCANCODE_INTERNATIONAL1:
      case SDL_SCANCODE_INTERNATIONAL2:
      case SDL_SCANCODE_INTERNATIONAL3:
      case SDL_SCANCODE_INTERNATIONAL4:
      case SDL_SCANCODE_INTERNATIONAL5:
      case SDL_SCANCODE_INTERNATIONAL6:
      case SDL_SCANCODE_LANG1:
      case SDL_SCANCODE_LANG2:
      case SDL_SCANCODE_LANG3:
      case SDL_SCANCODE_LANG4:
      case SDL_SCANCODE_LANG5:
      case SDL_SCANCODE_KP_LEFTPAREN:
      case SDL_SCANCODE_KP_RIGHTPAREN:
      case SDL_SCANCODE_KP_LEFTBRACE:
      case SDL_SCANCODE_KP_RIGHTBRACE:
      case SDL_SCANCODE_LCTRL:
      case SDL_SCANCODE_LSHIFT:
      case SDL_SCANCODE_LALT:
      case SDL_SCANCODE_LGUI:
      case SDL_SCANCODE_RCTRL:
      case SDL_SCANCODE_RSHIFT:
      case SDL_SCANCODE_RALT:
      case SDL_SCANCODE_RGUI:
        return InputCode(a_key);
      default:
        return IC_UNKNOWN;
    }
  }

  static InputCode TranslateMouseButtonCode(int a_code)
  {
    switch (a_code)
    {
      case SDL_BUTTON_LEFT:
        return IC_MOUSE_BUTTON_LEFT;
      case SDL_BUTTON_MIDDLE:
        return IC_MOUSE_BUTTON_MIDDLE;
      case SDL_BUTTON_RIGHT:
        return IC_MOUSE_BUTTON_RIGHT;
      case SDL_BUTTON_X1:
        return IC_MOUSE_BUTTON_X1;
      case SDL_BUTTON_X2:
        return IC_MOUSE_BUTTON_X2;
      default:
        return IC_UNKNOWN;
    }
  }

  static TRef<Message> TranslateWindowEvent(SDL_WindowEvent const & a_event)
  {
    switch (a_event.event)
    {
      case  SDL_WINDOWEVENT_SHOWN:
        return StaticPointerCast<Message>(TRef<Message_Window_Shown>::New());
      case  SDL_WINDOWEVENT_HIDDEN:
        return StaticPointerCast<Message>(TRef<Message_Window_Hidden>::New());
      case  SDL_WINDOWEVENT_EXPOSED:
        return StaticPointerCast<Message>(TRef<Message_Window_Exposed>::New());
      case  SDL_WINDOWEVENT_MOVED:
      {
        TRef<Message_Window_Moved> pMsg = TRef<Message_Window_Moved>::New();
        pMsg->x = a_event.data1;
        pMsg->y = a_event.data2;
        return StaticPointerCast<Message>(pMsg);
      }
      case  SDL_WINDOWEVENT_SIZE_CHANGED:
      //case  SDL_WINDOWEVENT_RESIZED: // Use SDL_WINDOWEVENT_SIZE_CHANGED instead
      {
        TRef<Message_Window_Resized> pMsg = TRef<Message_Window_Resized>::New();
        pMsg->w = a_event.data1;
        pMsg->h = a_event.data2;
        return StaticPointerCast<Message>(pMsg);
      }
      case  SDL_WINDOWEVENT_MINIMIZED:
        return StaticPointerCast<Message>(TRef<Message_Window_Minimized>::New());
      case  SDL_WINDOWEVENT_MAXIMIZED:
        return StaticPointerCast<Message>(TRef<Message_Window_Maximized>::New());
      case  SDL_WINDOWEVENT_RESTORED:
        return StaticPointerCast<Message>(TRef<Message_Window_Restored>::New());
      case  SDL_WINDOWEVENT_ENTER:
        return StaticPointerCast<Message>(TRef<Message_Window_Enter>::New());
      case  SDL_WINDOWEVENT_LEAVE:
        return StaticPointerCast<Message>(TRef<Message_Window_Leave>::New());
      case  SDL_WINDOWEVENT_FOCUS_GAINED:
        return StaticPointerCast<Message>(TRef<Message_Window_Focus_Gained>::New());
      case  SDL_WINDOWEVENT_FOCUS_LOST:
        return StaticPointerCast<Message>(TRef<Message_Window_Focus_Lost>::New());
      case  SDL_WINDOWEVENT_CLOSE:
        return StaticPointerCast<Message>(TRef<Message_Quit>::New());
      case  SDL_WINDOWEVENT_TAKE_FOCUS:
        return StaticPointerCast<Message>(TRef<Message_Window_Take_Focus>::New());
      default:
        return StaticPointerCast<Message>(TRef<Message_None>::New());
    }

    return StaticPointerCast<Message>(TRef<Message_None>::New());
  }

  TRef<Message> FW_EventPoller::NextEvent()
  {
    while (true)
    {
      SDL_Event event;
      if (SDL_PollEvent(&event) == 0)
        break;

      switch (event.type)
      {
        case SDL_TEXTINPUT:
        {
          TRef<Message_Input_Text> pMsg = TRef<Message_Input_Text>::New();
          pMsg->code = IC_TEXT;
          pMsg->event = IE_VALUE_CHANGE;
          pMsg->modState = GetModState();
          strncpy_s(pMsg->text, event.text.text, TEXT_INPUT_TEXT_SIZE);
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_KEYDOWN:
        {
          if (event.key.repeat)
            break;

          TRef<Message_Input> pMsg = TRef<Message_Input>::New();
          pMsg->code = TranslateKeyCode(event.key.keysym.scancode);
          pMsg->event = IE_BUTTON_DOWN;
          pMsg->modState = GetModState();
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_KEYUP:
        {
          TRef<Message_Input> pMsg = TRef<Message_Input>::New();
          pMsg->code = TranslateKeyCode(event.key.keysym.scancode);
          pMsg->event = IE_BUTTON_UP;
          pMsg->modState = GetModState();
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_MOUSEBUTTONDOWN:
        {
          TRef<Message_Input_MouseEvent> pMsg = TRef<Message_Input_MouseEvent>::New();
          pMsg->code = TranslateMouseButtonCode(event.button.button);
          pMsg->event = IE_BUTTON_DOWN;
          pMsg->modState = GetModState();
          pMsg->x = event.button.x;
          pMsg->y = event.button.y;
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_MOUSEBUTTONUP:
        {
          TRef<Message_Input_MouseEvent> pMsg = TRef<Message_Input_MouseEvent>::New();
          pMsg->code = TranslateMouseButtonCode(event.button.button);
          pMsg->event = IE_BUTTON_UP;
          pMsg->modState = GetModState();
          pMsg->x = event.button.x;
          pMsg->y = event.button.y;
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_MOUSEWHEEL:
        {
          TRef<Message_Input> pMsg = TRef<Message_Input>::New();
          pMsg->event = IE_VALUE_CHANGE;
          pMsg->modState = GetModState();
          if (event.wheel.y > 0)
            pMsg->code = IC_MOUSE_WHEEL_UP;
          else
            pMsg->code = IC_MOUSE_WHEEL_DOWN;
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_MOUSEMOTION:
        {
          TRef<Message_Input> pMsg = TRef<Message_Input>::New();
          pMsg->code = IC_MOUSE_MOTION;
          pMsg->event = IE_VALUE_CHANGE;
          pMsg->modState = GetModState();
          if (SDL_GetRelativeMouseMode() == SDL_TRUE)
          {
            pMsg->mousePosition.x = event.motion.xrel;
            pMsg->mousePosition.y = event.motion.yrel;
          }
          else
          {
            pMsg->mousePosition.x = event.motion.x;
            pMsg->mousePosition.y = event.motion.y;
          }
          return StaticPointerCast<Message>(pMsg);
        }
        case SDL_WINDOWEVENT:
        {
          return TranslateWindowEvent(event.window);
        }
        case SDL_QUIT:
        {
          return StaticPointerCast<Message>(TRef<Message_Quit>::New());
        }
        default:
        {
          break;
        }
      }
    }
    return TRef<Message>();
  }
}
