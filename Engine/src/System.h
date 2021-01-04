//@group Systems

#ifndef EN_SYSTEM_H
#define EN_SYSTEM_H

#include <stdint.h>
#include "MessageHandler.h"
#include "Memory.h"

#define MAKE_SYSTEM_DECL static ::Engine::System::ID GetStaticID();\
::Engine::System::ID GetID() const override;

#define MAKE_SYSTEM_DEFINITION(CLASS) ::Engine::System::ID CLASS::GetStaticID()\
{\
  static ID s_ID = 0;\
  if (s_ID == 0)\
    s_ID = _GetNewID();\
  return s_ID;\
}\
::Engine::System::ID CLASS::GetID() const\
{\
  return GetStaticID();\
}

namespace Engine
{
  class MessageBus;

  class System : public MessageHandler
  {
  public: 

    typedef uint32_t ID;

    virtual ~System(){}

    virtual ID GetID() const = 0;

    virtual void OnAttach(){}
    virtual void OnDetach(){}

    virtual void Update(float dt) =0;
    virtual void Render() {}
    virtual void DoImGui(){}

  protected:

    static ID _GetNewID();

    //System(System const &);
    //System & operator=(System const &);
  };
}

#endif