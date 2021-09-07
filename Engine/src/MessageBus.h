//@group Messages

#ifndef EN_MESSAGEBUS_H
#define EN_MESSAGEBUS_H

#include <stdint.h>
#include <mutex>

#include "MemBuffer.h"
#include "Message.h"
#include "PODArray.h"
#include "Memory.h"

#define POST(msg) ::DgE::MessageBus::Instance()->Register(msg)
#define EMPLACE_POST(...) CALL_OVERLOAD(EMPLACE_POST, __VA_ARGS__)

// TODO check for nullptr
#define EMPLACE_POST2(msgType, ptr) ptr = static_cast<msgType*>(::DgE::MessageBus::Instance()->_ReserveAndRegister(sizeof(msgType))); new (ptr) msgType()
#define EMPLACE_POST1(msgType) new (static_cast<msgType*>(::DgE::MessageBus::Instance()->_ReserveAndRegister(sizeof(msgType)))) msgType()

#define GLUE(x, y) x y

#define RETURN_ARG_COUNT(_1_, _2_, _3, count, ...) count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX5(...) EXPAND_ARGS((__VA_ARGS__, 3, 2, 1, 0))

#define OVERLOAD_MACRO2(name, count) name##count
#define OVERLOAD_MACRO1(name, count) OVERLOAD_MACRO2(name, count)
#define OVERLOAD_MACRO(name, count) OVERLOAD_MACRO1(name, count)

#define CALL_OVERLOAD(name, ...) GLUE(OVERLOAD_MACRO(name, COUNT_ARGS_MAX5(__VA_ARGS__)), (__VA_ARGS__))

namespace DgE
{
  class SystemStack;

  class MessageBus
  {
    static MessageBus * s_instance;

    MessageBus();

  public:

    static void Init();
    static void ShutDown();
    static MessageBus * Instance();

    // Allocate a message directly on the MessageBus memory heap. Access through macro above
    void * _ReserveAndRegister(size_t MsgSize);

    // Add message to the queue to be processed at a later time.
    // Can be used from any thread.
    void Register(TRef<Message> const &);

    // Since dispatching messages may generate more messages, we add an upper limit of how 
    // many times we want to cycle through the message buffers. 0 == no limit.
    void DispatchMessages(SystemStack &, uint32_t cycles = 0);

    size_t MessageCount();

  private:

    std::mutex              m_mutex;
    int                     m_writeBuffer;
    PODArray<Message*>      m_messageQueue[2];
    MemBuffer               m_buf[2];
  };
}

#endif