//@group Systems

#ifndef EN_SYSTEM_H
#define EN_SYSTEM_H

#include <stdint.h>
#include "MessageHandler.h"
#include "Memory.h"

#define MAKE_SYSTEM_DECL(id) inline static ::Engine::SystemID GetStaticID() {return id;}\
inline ::Engine::SystemID GetID() const override {return id;};

namespace Engine
{
  typedef uint32_t SystemID;

  enum ReservedSystemID : SystemID
  {
    RSID_Application,
    RSID_Console,
    RSID_GUI,
    RSID_Input,
    RSID_Window,
    RSID_BEGIN // Your System IDs must not be less than this value
  };

  class MessageBus;

  class System : public MessageHandler
  {
  public: 

    virtual ~System(){}

    virtual SystemID GetID() const = 0;

    virtual void OnAttach(){}
    virtual void OnDetach(){}

    virtual void Update(float dt) =0;
    virtual void Render() {}

  protected:

    //System(System const &);
    //System & operator=(System const &);
  };
}

#endif