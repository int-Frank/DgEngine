//@group Messages

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <string>
#include <type_traits>

#include "Utils.h"
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

    Message();

    bool QueryFlag(Flag a_flag) const;
    void SetFlag(Flag a_flag, bool);
    uint32_t GetCategory() const;

    // All these defined in the macro (user do NOT implement)
    virtual uint32_t GetID() const = 0;
    virtual size_t Size() const = 0;
    virtual TRef<Message> CloneAsTRef() const = 0;
    virtual void Clone(void *) const = 0; //Clone at a memory location

    // User defined
    virtual std::string ToString() const = 0;

  protected:
    uint32_t m_flags;
    static uint32_t GetNewID(uint32_t a_class);
  };
  static_assert(std::is_trivially_destructible<Message>::value, "Message must be trivially destructible");

#define MESSAGE_HEADER \
  private:\
    static uint32_t s_ID;\
  public:\
    static uint32_t GetStaticID();\
    uint32_t GetID() const override;\
    size_t Size() const override;\
    TRef<Message> CloneAsTRef() const override;\
    void Clone(void * a_buf) const override;\
    std::string ToString() const override;

  #define MESSAGE_DEFINITIONS(TYPE, CLASS) uint32_t TYPE::s_ID(0);\
  static_assert(std::is_trivially_destructible<TYPE>::value, #TYPE " must be trivially destructible");\
  uint32_t TYPE::GetStaticID()\
  {\
    if (s_ID == 0)\
      s_ID = GetNewID(CLASS);\
    return s_ID;\
  }\
  uint32_t TYPE::GetID() const\
  {\
    return GetStaticID();\
  }\
  size_t TYPE::Size() const\
  {\
    return sizeof(*this);\
  }\
  TRef<Message> TYPE::CloneAsTRef() const\
  {\
    TRef<TYPE> cpy = TRef<TYPE>::MakeCopy(this); \
    return StaticPointerCast<Message>(cpy); \
  }\
  void TYPE::Clone(void * a_buf) const\
  {\
    new (a_buf) TYPE(*this);\
  }

  class Message_Command : public Message
  {
    MESSAGE_HEADER

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
}
#endif