//@group Systems

#ifndef EN_SYSTEM_H
#define EN_SYSTEM_H

#include <stdint.h>
#include "MessageHandler.h"
#include "Memory.h"

#define ASSIGN_ID(id) ::Engine::System::ID GetID() override {return id;}\
static ::Engine::System::ID GetStaticID() {return id;}

namespace Engine
{
  class MessageBus;

  enum class DefaultSystem
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
  class System : public MessageHandler
  {
  public: 

    typedef uint32_t ID;

    virtual ~System(){}

    virtual ID GetID() {return 0;}

    virtual void OnAttach(){}
    virtual void OnDetach(){}

    virtual void Update(float dt) =0;
    virtual void Render() {}
    virtual void DoImGui(){}

  protected:

    static ID s_currentID;
    static ID GetNextID();

    //System(System const &);
    //System & operator=(System const &);
  };
}

#endif