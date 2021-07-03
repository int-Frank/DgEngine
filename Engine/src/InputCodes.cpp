//@group Core

#include "InputCodes.h"

namespace Engine
{
  uint16_t const InputKeyMap[nIC_COUNT + 1] =
  {
#undef ITEM
#define ITEM(name, val) IC_ ## name,
    INPUT_CODES
  };
}