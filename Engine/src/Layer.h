//@group Layers

#ifndef EN_LAYER_H
#define EN_LAYER_H

#include <stdint.h>
#include "MessageHandler.h"
#include "Memory.h"

#define ASSIGN_ID(id) ::Engine::Layer::ID GetID() override {return id;}\
static ::Engine::Layer::ID GetStaticID() {return id;}

namespace Engine
{
  class MessageBus;

  enum class DefaultLayer
  {
    Application,
    Console,
    UI,
    InputHandler,
    Window,
  };

  //TODO Subscribers should collect messages and process them on a single call
  //     to Update(). This way, we can throw the subscriber on a separate thread
  //     and update.
  class Layer : public MessageHandler
  {
  public: 

    typedef uint32_t ID;

    virtual ~Layer(){}

    virtual ID GetID() {return 0;}

    virtual void OnAttach(){}
    virtual void OnDetach(){}

    virtual void Update(float dt) =0;
    virtual void Render() {}
    virtual void DoImGui(){}

  protected:

    static ID s_currentID;
    static ID GetNextID();

    //Layer(Layer const &);
    //Layer & operator=(Layer const &);
  };
}

#endif