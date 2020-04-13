//@group Layers

#include "Layer_Console.h"
#include "core_Log.h"

#define LOG_MESSAGE(...) LOG_TRACE(__VA_ARGS__)

namespace Engine
{
  Layer_Console::Layer_Console()
  {

  }

  Layer_Console::~Layer_Console()
  {

  }

  void Layer_Console::Update(float a_dt)
  {

  }

  void Layer_Console::HandleMessage(Message * a_pMsg)
  {
    if (a_pMsg->GetCategory() != MC_Input)
      return;

  }
}