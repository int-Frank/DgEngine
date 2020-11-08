//@group Layers

#ifndef EN_LAYER_INPUTHANDLER_H
#define EN_LAYER_INPUTHANDLER_H

#include <stdint.h>
#include <initializer_list>

#include "DgMap_AVL.h"

#include "Memory.h"
#include "MessageBus.h"
#include "EngineMessages.h"
#include "Layer.h"
#include "InputCodes.h"

namespace Engine
{
  class IEventPoller;

  typedef void (*InputMessageTranslator)(Message const *);

  class Layer_InputHandler : public Layer
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::InputHandler))

    Layer_InputHandler();
    ~Layer_InputHandler();

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