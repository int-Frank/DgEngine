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

namespace DgE
{
  class IEventPoller;

  typedef void (*InputMessageTranslator)(Message const *, void * pData);

  class System_Input : public System
  {
  public:

    MAKE_SYSTEM_DECL

    System_Input();
    ~System_Input();

    void ClearBindings();
    void Update(float);

    void HandleMessage(Message*) override;

    void AddBinding(InputCode, InputEvent, InputMessageTranslator, void *pData = nullptr);

  private:

    struct Data
    {
      InputMessageTranslator func;
      void * pData;
    };

    IEventPoller     * m_pEventPoller;
    std::vector<uint32_t> keyFlags;
    Dg::Map_AVL<uint32_t, Data> m_bindings;

  };
}


#endif