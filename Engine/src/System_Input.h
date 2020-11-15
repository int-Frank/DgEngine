//@group Systems

#ifndef EN_SYSTEM_INPUT_H
#define EN_SYSTEM_INPUT_H

#include <stdint.h>
#include <initializer_list>

#include "DgMap_AVL.h"

#include "Memory.h"
#include "MessageBus.h"
#include "EngineMessages.h"
#include "System.h"
#include "InputCodes.h"

namespace Engine
{
  class IEventPoller;

  typedef void (*InputMessageTranslator)(Message const *);

  class System_Input : public System
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultSystem::InputHandler))

    System_Input();
    ~System_Input();

    void ClearBindings();
    void Update(float);

    void HandleMessage(Message*) override;

    // LayerHandler will own the input Message *
    void AddBinding(uint32_t inputMessageID, InputMessageTranslator);

  private:

    IEventPoller     * m_pEventPoller;
    Dg::Map_AVL<uint32_t, InputMessageTranslator> m_bindings;

  };
}


#endif