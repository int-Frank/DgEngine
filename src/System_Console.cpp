//@group Systems

#include "System_Console.h"
#include "Log.h"

#define IGNORE_MESSAGE(TYPE) if (a_pMsg->GetID() == TYPE::GetStaticID()) return
#define LOG_MESSAGE(...) LOG_DEBUG(__VA_ARGS__)

namespace DgE
{
  MAKE_SYSTEM_DEFINITION(System_Console)

  System_Console::System_Console()
  {

  }

  System_Console::~System_Console()
  {

  }

  void System_Console::Update(float a_dt)
  {

  }

  void System_Console::HandleMessage(Message * a_pMsg)
  {
    IGNORE_MESSAGE(Message_GUI_PointerMove);
    LOG_MESSAGE("MSG: {}", a_pMsg->ToString());
  }
}