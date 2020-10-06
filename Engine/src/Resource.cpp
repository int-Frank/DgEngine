
#include <climits>

#include "Resource.h"

namespace Engine
{
  namespace impl
  {
    ResourceID GetNextID()
    {
      static ResourceID s_ID = 0;
      ResourceID id = ++s_ID;
      id = (1 << (sizeof(ResourceID) * CHAR_BIT - 1) | id);
      return id;
    }
  }
}