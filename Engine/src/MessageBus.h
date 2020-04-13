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

namespace Engine
{
  class Layer;
  class LayerStack;

  class MessageBus
  {
    static size_t const s_bufSize = 1 * 1024 * 1024;
    static MessageBus * s_instance;

    MessageBus(LayerStack&);

  public:

    static void Init(LayerStack&);
    static void ShutDown();
    static MessageBus * Instance();

    //TODO maybe have an 'Emplace' method, so we can construct messages
    //     directly on the MessageBus memory heap.

    //Add message to the queue to be processed at a later time.
    //Can be used from any thread.
    void Register(TRef<Message> const &);

    //SwapBuffers needs to be called before DispatchMessages, at a time no
    //Messages are being registered.
    void SwapBuffers();
    void DispatchMessages();

    size_t MessageCount();

  private:

    std::mutex              m_mutex;
    int                     m_producerIndex;
    PODArray<Message*>      m_messageQueue[2];
    MemBuffer               m_buf[2];
    LayerStack &            m_layerStack;
  };
}

#endif