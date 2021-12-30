//@group Systems

#ifndef EN_SYSTEM_H
#define EN_SYSTEM_H

#include <stdint.h>
#include "utils.h"
#include "MessageHandler.h"
#include "Memory.h"

#define MAKE_SYSTEM_DECL static ::DgE::SystemID GetStaticID();\
::DgE::SystemID GetID() const override;

#define MAKE_SYSTEM_DEFINITION(CLASS) ::DgE::SystemID CLASS::GetStaticID()\
{\
  static ::DgE::SystemID s_ID = 0;\
  if (s_ID == 0)\
    s_ID = _GetNewID();\
  return s_ID;\
}\
::DgE::SystemID CLASS::GetID() const\
{\
  return GetStaticID();\
}

namespace DgE
{
  class MessageBus;

  typedef uint32_t SystemID;

  class System : public MessageHandler
  {
  public: 

    virtual ~System(){}

    virtual ::DgE::SystemID GetID() const = 0;

    virtual void OnAttach(){}
    virtual void OnDetach(){}

    virtual void Update(float dt) =0;
    virtual void Render() {}

  protected:

    static SystemID _GetNewID();

    //System(System const &);
    //System & operator=(System const &);
  };
}

#endif