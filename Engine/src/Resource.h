//@group Memory

#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>

namespace Engine
{
  // Ensure the last bit is 0 for your game IDs.
  // Internal IDs are identified by the last bit set to 1.
  typedef uint32_t ResourceID;
#define INVALID_RESOURCE_ID 0xFFFFFFFF

  namespace impl
  {
    // Internal use
    ResourceID GetNextID();
  }
}

#endif