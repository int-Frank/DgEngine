//@group Messages

#ifndef EN_MESSAGEBUS_H
#define EN_MESSAGEBUS_H

#include <stdint.h>
#include <mutex>

#include "MemBuffer.h"
#include "Message.h"
#include "PODArray.h"
#include "Memory.h"

#define POST(msg) ::Engine::MessageBus::Instance()->Register(msg)
#define EMPLACE_POST(...) CALL_OVERLOAD(EMPLACE_POST, __VA_ARGS__)

// TODO check for nullptr
#define EMPLACE_POST2(msgType, ptr) ptr = static_cast<msgType*>(::Engine::MessageBus::Instance()->_ReserveAndRegister(sizeof(msgType))); new (ptr) msgType()
#define EMPLACE_POST1(msgType) new (static_cast<msgType*>(::Engine::MessageBus::Instance()->_ReserveAndRegister(sizeof(msgType)))) msgType()

#define GLUE(x, y) x y

#define RETURN_ARG_COUNT(_1_, _2_, _3, count, ...) count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX5(...) EXPAND_ARGS((__VA_ARGS__, 3, 2, 1, 0))

#define OVERLOAD_MACRO2(name, count) name##count
#define OVERLOAD_MACRO1(name, count) OVERLOAD_MACRO2(name, count)
#define OVERLOAD_MACRO(name, count) OVERLOAD_MACRO1(name, count)

#define CALL_OVERLOAD(name, ...) GLUE(OVERLOAD_MACRO(name, COUNT_ARGS_MAX5(__VA_ARGS__)), (__VA_ARGS__))


namespace Engine
{
  class System;
  class SystemStack;

  class MessageBus
  {
    static size_t const s_bufSize = 1 * 1024 * 1024;
    static MessageBus * s_instance;

    MessageBus(SystemStack&);

  public:

    static void Init(SystemStack&);
    static void ShutDown();
    static MessageBus * Instance();

    // Allocate a message directly on the MessageBus memory heap. Access through macro above
    void * _ReserveAndRegister(size_t MsgSize);

    // Add message to the queue to be processed at a later time.
    // Can be used from any thread.
    void Register(TRef<Message> const &);

    // SwapBuffers needs to be called before DispatchMessages, at a time no
    // Messages are being registered.
    void SwapBuffers();
    void DispatchMessages();

    size_t MessageCount();

  private:

    std::mutex              m_mutex;
    int                     m_producerIndex;
    PODArray<Message*>      m_messageQueue[2];
    MemBuffer               m_buf[2];
    SystemStack &           m_systemStack;
  };
}

#endif