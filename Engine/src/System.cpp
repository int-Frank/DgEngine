//@group Systems

#include "System.h"

namespace Engine
{
  System::ID System::_GetNewID()
  {
    static ID s_currentID = 0;
    return ++s_currentID;
  }
}