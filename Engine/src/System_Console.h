//@group Systems

#ifndef EN_SYSTEM_CONSOLE_H
#define EN_SYSTEM_CONSOLE_H

#include "EngineMessages.h"
#include "System.h"

//Perhaps we can remove this from the Engine. It really belongs in the Game project.
namespace Engine
{
  class System_Console : public System
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultSystem::Console))

    System_Console();
    ~System_Console();

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Command*);

#undef ITEM
#define ITEM(TYPE, CATEGORY) void HandleMessage(Message_##TYPE *);
    MESSAGE_LIST;

    void Update(float);

  };
}

#endif