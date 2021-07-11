//@group Systems

#include "System.h"

namespace Engine
{
  SystemID System::_GetNewID()
  {
    static SystemID s_currentID = 0;
    return ++s_currentID;
  }
}