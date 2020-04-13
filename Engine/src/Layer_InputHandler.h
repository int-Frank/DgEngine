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
  class IMouseController;

  class InputBinding
  {
  public:

    InputCode code;
    InputEvent evnt;
    Message * pMsg;

    InputBinding(InputCode, InputEvent, Message*);
    InputBinding(InputCode, Message*);
  };

  class Layer_InputHandler : public Layer
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::InputHandler))

    Layer_InputHandler();
    ~Layer_InputHandler();

    void ClearBindings();
    void Update(float);

    void GrabMouse();
    void ReleaseMouse();
    void SetMouseLookRate(float xRate, float yRate);

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Input_Text*);
    void HandleMessage(Message_Input_KeyUp *);
    void HandleMessage(Message_Input_KeyDown *);
    void HandleMessage(Message_Input_MouseButtonUp *);
    void HandleMessage(Message_Input_MouseButtonDown *);
    void HandleMessage(Message_Input_MouseWheelUp *);
    void HandleMessage(Message_Input_MouseWheelDown *);
    void HandleMessage(Message_Input_MouseMove *);

    void SetBindings(std::initializer_list<InputBinding> const &);

  private:

    typedef uint32_t BindingKey;

    void HandleBinding(BindingKey key, Message const * source);

    BindingKey PackKey(InputCode, InputEvent);

    Ref<IEventPoller>                   m_eventPoller;
    Ref<IMouseController>               m_mouseController;

    float                                           m_xMouseRotRate;
    float                                           m_yMouseRotRate;
    Dg::Map_AVL<uint64_t, Ref<Message>> m_bindings;

  };
}


#endif