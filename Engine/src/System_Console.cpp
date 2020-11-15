//@group Systems

#include "System_Console.h"
#include "core_Log.h"

#define LOG_MESSAGE(...) LOG_TRACE(__VA_ARGS__)

namespace Engine
{
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
    if (a_pMsg->GetCategory() != MC_Input)
      return;

  }
}