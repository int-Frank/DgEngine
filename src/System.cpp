//@group Systems

#include "System.h"

namespace DgE
{
  SystemID System::_GetNewID()
  {
    static SystemID s_currentID = 0;
    return ++s_currentID;
  }
}