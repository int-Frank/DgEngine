//@group Messages

#include <cstring>
#include <new>
#include <sstream>

#include "EngineMessages.h"

#define ID_SHIFT 20

// This marco needs to be somehwere the client can access
#undef ITEM
#define ITEM(TYPE, CLASS) uint32_t Message_##TYPE::s_ID(0);\
static_assert(std::is_trivially_destructible<Message_##TYPE>::value, #TYPE " must be trivially destructible");\
uint32_t Message_##TYPE::GetStaticID()\
{\
  if (s_ID == 0)\
    s_ID = GetNewID(MC_##CLASS);\
  return s_ID;\
}\
uint32_t Message_##TYPE::GetID() const\
{\
  return GetStaticID();\
}\
size_t Message_##TYPE::Size() const\
{\
  return sizeof(*this);\
}\
TRef<Message> Message_##TYPE::CloneAsTRef() const\
{\
  TRef<Message_##TYPE> cpy = TRef<Message_##TYPE>::MakeCopy(this); \
  return StaticPointerCast<Message>(cpy); \
}\
void Message_##TYPE::Clone(void * a_buf) const\
{\
  new (a_buf) Message_##TYPE(*this);\
}

#define MESSAGE_TO_STRING(TYPE) std::string Message_##TYPE::ToString() const\
{\
  return std::string(#TYPE);\
}

namespace Engine
{
  uint32_t Message::GetNewID(uint32_t a_group)
  {
    static uint32_t s_counter = 0;
    uint32_t idTail = ++s_counter;
    return (a_group << ID_SHIFT) | idTail;
  }

  Message::Message() 
    : m_flags(0)
  {

  }

  bool Message::QueryFlag(Flag a_flag) const
  {
    return (m_flags & static_cast<uint32_t>(a_flag)) != 0;
  }

  void Message::SetFlag(Flag a_flag, bool a_on)
  {
    if (a_on)
      m_flags = (m_flags | static_cast<uint32_t>(a_flag));
    else
      m_flags = (m_flags & ~static_cast<uint32_t>(a_flag));
  }

  uint32_t Message::GetCategory() const
  {
    return GetID() >> ID_SHIFT;
  }

  MESSAGE_LIST

  MESSAGE_TO_STRING(None)
  MESSAGE_TO_STRING(GUI_GoBack)
  MESSAGE_TO_STRING(GUI_Up)
  MESSAGE_TO_STRING(GUI_Down)
  MESSAGE_TO_STRING(GUI_Left)
  MESSAGE_TO_STRING(GUI_Right)
  MESSAGE_TO_STRING(GUI_Select)
  MESSAGE_TO_STRING(GUI_Text)
  MESSAGE_TO_STRING(Window_Shown)
  MESSAGE_TO_STRING(Window_Hidden)
  MESSAGE_TO_STRING(Window_Exposed)
  MESSAGE_TO_STRING(Window_Minimized)
  MESSAGE_TO_STRING(Window_Maximized)
  MESSAGE_TO_STRING(Window_Restored)
  MESSAGE_TO_STRING(Window_Enter)
  MESSAGE_TO_STRING(Window_Leave)
  MESSAGE_TO_STRING(Window_Focus_Gained)
  MESSAGE_TO_STRING(Window_Focus_Lost)
  MESSAGE_TO_STRING(Window_Close)
  MESSAGE_TO_STRING(Window_Take_Focus)
  MESSAGE_TO_STRING(Input_MouseWheelUp)
  MESSAGE_TO_STRING(Input_MouseWheelDown)

    std::string Message_GUI_PointerDown::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_PointerDown [context: " << context << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_GUI_PointerUp::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_PointerUp [context: " << context << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_GUI_PointerMove::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_PointerMov [x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_Window_Moved::ToString() const
  {
    std::stringstream ss;
    ss << "Window_Moved [x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_Window_Resized::ToString() const
  {
    std::stringstream ss;
    ss << "Window_Resized [w: " << w << ", h: " << h << "]";
    return ss.str();
  }

  std::string Message_Window_Size_Changed::ToString() const
  {
    std::stringstream ss;
    ss << "Window_Size_Changed [w: " << w << ", h: " << h << "]";
    return ss.str();
  }

  std::string Message_Input_Text::ToString() const
  {
    std::stringstream ss;
    ss << "Input_Text [text: PLACEHOLDER]";
    return ss.str();
  }

  std::string Message_Input_KeyUp::ToString() const
  {
    std::stringstream ss;
    ss << "Input_KeyUp [keycode: " << keyCode << ", mod state " << modState << "]";
    return ss.str();
  }

  std::string Message_Input_KeyDown::ToString() const
  {
    std::stringstream ss;
    ss << "Input_KeyDown [keycode: " << keyCode << ", mod state " << modState << "]";
    return ss.str();
  }

  std::string Message_Input_MouseButtonUp::ToString() const
  {
    std::stringstream ss;
    ss << "Input_MouseButtonUp [button: " << button << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_Input_MouseButtonDown::ToString() const
  {
    std::stringstream ss;
    ss << "Input_MouseButtonDown [button: " << button << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_Input_MouseMove::ToString() const
  {
    std::stringstream ss;
    ss << "Input_MouseMove [x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  Message_Command::Message_Command()
    : ptr(nullptr)
  {
  
  }

  Message_Command::Message_Command(void* a_ptr)
    : ptr(a_ptr)
  {

  }

  uint32_t Message_Command::s_ID(0);

  uint32_t Message_Command::GetStaticID()
  {
    if (s_ID == 0)
      s_ID = GetNewID(MC_None);
    return s_ID;
  }

  uint32_t Message_Command::GetID() const
  {
    return GetStaticID();
  }

  size_t Message_Command::Size() const
  {
    size_t sze = sizeof(Message_Command);
    if (ptr)
      sze += *static_cast<uint64_t*>(ptr);
    return sze;
  }

  TRef<Message> Message_Command::CloneAsTRef() const
  {
    TRef<Message_Command> cpy = TRef<Message_Command>::MakeCopy(this);
    return StaticPointerCast<Message>(cpy);
  }

  void Message_Command::Clone(void* a_buf) const
  {
    void* pData = Core::AdvancePtr(a_buf, sizeof(Message_Command));
    new (a_buf) Message_Command(pData);
    if (ptr)
    {
      uint64_t sze = *static_cast<uint64_t*>(ptr);
      memcpy(pData, ptr, sze);
    }
  }

  std::string Message_Command::ToString() const
  {
    std::stringstream ss;
    ss << "Message_Command [ptr: " << ptr << "]";
    return ss.str();
  }

  void Message_Command::Run()
  {
    if (!ptr)
      return;

    void * _ptr = Core::AdvancePtr(ptr, sizeof(uint64_t));

    MessageCommandFn function = *(MessageCommandFn*)_ptr;
    _ptr = Core::AdvancePtr(_ptr, sizeof(MessageCommandFn));

    function(_ptr);
  }
}