//@group Systems

#ifndef EN_SYSTEM_CONSOLE_H
#define EN_SYSTEM_CONSOLE_H

#include "EngineMessages.h"
#include "System.h"

//Perhaps we can remove this from the Engine. It really belongs in the Game project.
namespace DgE
{
  class System_Console : public System
  {
  public:

    MAKE_SYSTEM_DECL

    System_Console();
    ~System_Console();

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Command*);

    void Update(float);

  };
}

#endif