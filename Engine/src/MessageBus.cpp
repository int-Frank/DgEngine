//@group Messages

#include "Message.h"
#include "MessageBus.h"
#include "System.h"
#include "SystemStack.h"
#include "options.h"

namespace Engine
{
  MessageBus * MessageBus::s_instance = nullptr;

  void MessageBus::Init()
  {
    s_instance = new MessageBus();
  }

  void MessageBus::ShutDown()
  {
    delete s_instance;
    s_instance = nullptr;
  }

  MessageBus * MessageBus::Instance()
  {
    return s_instance;
  }

  MessageBus::MessageBus()
    : m_writeBuffer(0)
  {

  }

  size_t MessageBus::MessageCount()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_messageQueue[m_writeBuffer].size();
  }

  void * MessageBus::_ReserveAndRegister(size_t a_msgSize)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    void * buf = m_buf[m_writeBuffer].Allocate(a_msgSize);
    m_messageQueue[m_writeBuffer].push_back(static_cast<Message *>(buf));
    return buf;
  }

  void MessageBus::Register(TRef<Message> const & a_message)
  {
    size_t sze = a_message->Size();
    void * buf = nullptr;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      buf = m_buf[m_writeBuffer].Allocate(sze);
      m_messageQueue[m_writeBuffer].push_back(static_cast<Message *>(buf));
    }
    a_message->Clone(buf);
  }

  void MessageBus::DispatchMessages(SystemStack & a_systemStack, uint32_t a_cycles)
  {
    if (a_cycles == 0)
      a_cycles = 0xFFFFFFFF;

    for (uint32_t c = 0; c < a_cycles; c++)
    {
      int readBuffer = m_writeBuffer;
      m_writeBuffer = (m_writeBuffer + 1) & 1;

      if (m_messageQueue[readBuffer].size() == 0)
        break;

      for (size_t i = 0; i < m_messageQueue[readBuffer].size(); i++)
      {
        Message * pMsg = m_messageQueue[readBuffer][i];
        auto it = a_systemStack.begin();
        for (; it != a_systemStack.end(); it++)
        {
          it->second->HandleMessage(pMsg);
          if (pMsg->QueryFlag(Message::Flag::Handled))
            break;
        }
      }

      m_messageQueue[readBuffer].clear();
      m_buf[readBuffer].clear();
    }
  }
}
