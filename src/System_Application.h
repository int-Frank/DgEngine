//@group Systems

#ifndef EN_SYSTEM_APPLICATION_H
#define EN_SYSTEM_APPLICATION_H

#include "Memory.h"

#include "DgError.h"
#include "System.h"
#include "Memory.h"

namespace DgE
{
  class System_Application : public System
  {
  public:

    MAKE_SYSTEM_DECL

    System_Application();
    ~System_Application();

    void OnAttach();
    void OnDetach();

    void Update(float);

    void HandleMessage(Message *) override;
    void HandleMessage(Message_Command*);

  private:

  };
}


#endif