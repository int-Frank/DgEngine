//@group Messages

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <string>
#include <type_traits>

#include "core_utils.h"
#include "Memory.h"

namespace Engine
{
  //General Message command
  typedef void(*MessageCommandFn)(void*);

  enum MessageCategory : uint32_t
  {
    MC_None    = 0,
    MC_GUI,
    MC_Input,
    MC_Window,
    MC_CLIENT_BEGIN //Create your own categories beginning at this value
  };

  class Message
  {
  public:

    enum class Flag: uint32_t
    {
      Handled = BIT(0),
      Show    = BIT(1)
    };

  public:
    Message(): m_flags(0){}
    bool Is(Flag a_flag) const {return (m_flags & static_cast<uint32_t>(a_flag)) != 0;}
    void SetFlag(Flag a_flag) {m_flags = (m_flags | static_cast<uint32_t>(a_flag));}
    virtual uint32_t GetID() const = 0;
    uint32_t GetCategory() const;
    virtual size_t Size() const = 0;
    virtual TRef<Message> CloneAsTRef() const = 0; //Clone at a memory location
    virtual void Clone(void *) const = 0; //Clone at a memory location
    virtual std::string ToString() const = 0;

  protected:
    uint32_t m_flags;
  protected:
    static uint32_t GetNewID(uint32_t a_class);
  };
  static_assert(std::is_trivially_destructible<Message>::value, "Message must be trivially destructible");

#define MESSAGE_CLASS_HEADER(MESSAGE_TYPE) class Message_##MESSAGE_TYPE : public Message\
  {\
    static uint32_t s_ID;\
  public:\
    static uint32_t GetStaticID();\
    uint32_t GetID() const override;\
    size_t Size() const override;\
    TRef<Message> CloneAsTRef() const override;\
    void Clone(void * a_buf) const override;\
    std::string ToString() const override;

  MESSAGE_CLASS_HEADER(Command)
    template<typename FuncT>
    static TRef<Message> New(FuncT&& func)
    {
      static_assert(std::is_trivially_destructible<FuncT>::value, "FuncT must be trivially destructible");
      MessageCommandFn renderCmd = [](void* ptr)
      {
        auto pFunc = (FuncT*)ptr;
        (*pFunc)();
        pFunc->~FuncT();
      };

      size_t sze = sizeof(uint64_t) + sizeof(MessageCommandFn) + sizeof(func);
      void* pBuf = TBUFAlloc(sze);

      *static_cast<uint64_t*>(pBuf) = sze;

      void* ptr = AdvancePtr(pBuf, sizeof(uint64_t));
      *static_cast<MessageCommandFn*>(ptr) = renderCmd;

      ptr = AdvancePtr(ptr, sizeof(MessageCommandFn));
      new (ptr) FuncT(std::forward<FuncT>(func));

      TRef<Message_Command> msg = TRef<Message_Command>::New(pBuf);
      return DynamicPointerCast<Message>(msg);
    }

    Message_Command();
    Message_Command(void* a_ptr);

    void Run();

    void* ptr;
  };

  //-----------------------------------------------------------------------------------
  // Message translators
  //-----------------------------------------------------------------------------------
  
  namespace MessageTranslator
  {
    typedef void (*MessageTranslatorFn)(Message * dest, Message const * src);

    void Clear();
    bool Exists(uint32_t, uint32_t);
    void AddTranslator(uint32_t, uint32_t, MessageTranslatorFn); //Will override
    void Translate(Message * dest, Message const * src);
    void AddDefaultTranslators();
  }
}
#endif