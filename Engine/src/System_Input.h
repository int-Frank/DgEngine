//@group Systems

#ifndef EN_SYSTEM_INPUT_H
#define EN_SYSTEM_INPUT_H

#include <stdint.h>
#include <vector>

#include "DgMap_AVL.h"

#include "Memory.h"
#include "MessageBus.h"
#include "EngineMessages.h"
#include "System.h"
#include "InputCodes.h"

namespace Engine
{
  class IEventPoller;

  typedef void (*InputMessageTranslator)(Message_Input const *);

  class System_Input : public System
  {
  public:

    MAKE_SYSTEM_DECL(RSID_Input)

    System_Input();
    ~System_Input();

    void ClearBindings();
    void Update(float);

    void HandleMessage(Message*) override;

    void AddBinding(InputCode, InputEvent, InputMessageTranslator);

  private:

    IEventPoller     * m_pEventPoller;
    std::vector<uint32_t> keyFlags;
    Dg::Map_AVL<uint32_t, InputMessageTranslator> m_bindings;

  };
}


#endif