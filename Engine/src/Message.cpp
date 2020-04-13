//@group Messages

#include <cstring>
#include <mutex>
#include <new>
#include <sstream>

#include "EngineMessages.h"
#include "MessageHandler.h"
#include "DgMap_AVL.h"
#include "core_utils.h"

#define ID_SHIFT 20

#undef ITEM
#define ITEM(TYPE, CLASS) uint32_t Message_##TYPE::s_ID(0);\
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
  namespace impl
  {
    static std::mutex s_mutex;
    static uint32_t s_counter = 0;
  }

  uint32_t Message::GetNewID(uint32_t a_group)
  {
    std::lock_guard<std::mutex> lock(impl::s_mutex);
    impl::s_counter++;
    return (a_group << ID_SHIFT) | impl::s_counter;
  }

  uint32_t Message::GetCategory() const
  {
    return GetID() >> ID_SHIFT;
  }

  MESSAGE_LIST

  MESSAGE_TO_STRING(None)
  MESSAGE_TO_STRING(GoBack)
  MESSAGE_TO_STRING(GUI_MouseWheelUp)
  MESSAGE_TO_STRING(GUI_MouseWheelDown)
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
  
  std::string Message_GUI_MouseMove::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_MouseMove [x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_GUI_MouseButtonUp::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_MouseButtonUp [button: " << button << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_GUI_MouseButtonDown::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_MouseButtonDown [button: " << button << ", x: " << x << ", y: " << y << "]";
    return ss.str();
  }

  std::string Message_GUI_KeyUp::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_KeyUp [keycode: " << keyCode << ", mod state " << modState << "]";
    return ss.str();
  }

  std::string Message_GUI_KeyDown::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_KeyDown [keycode: " << keyCode << ", mod state " << modState << "]";
    return ss.str();
  }

  std::string Message_GUI_Text::ToString() const
  {
    std::stringstream ss;
    ss << "GUI_Text [text: PLACEHOLDER]";
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

  namespace MessageTranslator
  {
    static Dg::Map_AVL<uint64_t, MessageTranslatorFn> g_msgTrnsMap;

    static void Translate_GUI_MouseMove__Input_MouseMove(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_MouseMove * pDest 
        = static_cast<Message_GUI_MouseMove *> (a_pDest);
      Message_Input_MouseMove const* pSrc 
        = static_cast<Message_Input_MouseMove const *> (a_pSrc);

      pDest->x = pSrc->x;
      pDest->y = pSrc->y;
    }

    static void Translate_GUI_Text__Input_Text(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_Text * pDest 
        = static_cast<Message_GUI_Text *> (a_pDest);
      Message_Input_Text const* pSrc 
        = static_cast<Message_Input_Text const *> (a_pSrc);

      strncpy_s(pDest->text, TEXT_INPUT_TEXT_SIZE, pSrc->text, TEXT_INPUT_TEXT_SIZE);
    }

    static void Translate_GUI_KeyDown__Input_KeyDown(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_KeyDown * pDest 
        = static_cast<Message_GUI_KeyDown *> (a_pDest);
      Message_Input_KeyDown const* pSrc 
        = static_cast<Message_Input_KeyDown const *> (a_pSrc);

      pDest->keyCode = pSrc->keyCode;
      pDest->modState = pSrc->modState;
    }

    static void Translate_GUI_KeyUp__Input_KeyUp(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_KeyUp * pDest 
        = static_cast<Message_GUI_KeyUp *> (a_pDest);
      Message_Input_KeyUp const* pSrc 
        = static_cast<Message_Input_KeyUp const *> (a_pSrc);

      pDest->keyCode = pSrc->keyCode;
      pDest->modState = pSrc->modState;
    }

    static void Translate_GUI_MouseButtonDown__Input_MouseButtonDown(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_MouseButtonDown * pDest 
        = static_cast<Message_GUI_MouseButtonDown *> (a_pDest);
      Message_Input_MouseButtonDown const* pSrc 
        = static_cast<Message_Input_MouseButtonDown const *> (a_pSrc);

      pDest->button = pSrc->button;
      pDest->x = pSrc->x;
      pDest->y = pSrc->y;
    }

    static void Translate_GUI_MouseButtonUp__Input_MouseButtonUp(Message * a_pDest, Message const * a_pSrc)
    {
      Message_GUI_MouseButtonUp * pDest 
        = static_cast<Message_GUI_MouseButtonUp *> (a_pDest);
      Message_Input_MouseButtonUp const* pSrc 
        = static_cast<Message_Input_MouseButtonUp const *> (a_pSrc);

      pDest->button = pSrc->button;
      pDest->x = pSrc->x;
      pDest->y = pSrc->y;
    }

    static uint64_t PackKey(uint32_t a_dest, uint32_t a_src)
    {
      return (uint64_t(a_dest) << 32) | a_src;
    }

    void Clear()
    {
      g_msgTrnsMap.clear();
    }

    bool Exists(uint32_t a_dest, uint32_t a_src)
    {
      uint64_t key = PackKey(a_dest, a_src);
      return g_msgTrnsMap.find(key) != g_msgTrnsMap.end();
    }

    void Translate(Message * a_dest, Message const * a_src)
    {
      uint64_t key = PackKey(a_dest->GetID(), a_src->GetID());
      auto it = g_msgTrnsMap.find(key);
      if (it != g_msgTrnsMap.end())
        it->second(a_dest, a_src);
    }

    void AddTranslator(uint32_t a_dest, uint32_t a_src,
                       MessageTranslatorFn a_fn)
    {
      uint64_t key = PackKey(a_dest, a_src);
      g_msgTrnsMap[key] = a_fn;
    }

    void AddDefaultTranslators()
    {
      AddTranslator(Message_GUI_MouseMove::GetStaticID(), Message_Input_MouseMove::GetStaticID(), Translate_GUI_MouseMove__Input_MouseMove);
      AddTranslator(Message_GUI_Text::GetStaticID(), Message_Input_Text::GetStaticID(), Translate_GUI_Text__Input_Text);
      AddTranslator(Message_GUI_KeyDown::GetStaticID(), Message_Input_KeyDown::GetStaticID(), Translate_GUI_KeyDown__Input_KeyDown);
      AddTranslator(Message_GUI_KeyUp::GetStaticID(), Message_Input_KeyUp::GetStaticID(), Translate_GUI_KeyUp__Input_KeyUp);
      AddTranslator(Message_GUI_MouseButtonDown::GetStaticID(), Message_Input_MouseButtonDown::GetStaticID(), Translate_GUI_MouseButtonDown__Input_MouseButtonDown);
      AddTranslator(Message_GUI_MouseButtonUp::GetStaticID(), Message_Input_MouseButtonUp::GetStaticID(), Translate_GUI_MouseButtonUp__Input_MouseButtonUp);
    }
  }
}