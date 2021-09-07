//@group Core

#include "InputCodes.h"

namespace DgE
{
  char const * GetInputCodeString(InputCode a_code)
  {
#undef ITEM
#define ITEM(name, val) case IC_ ## name: {return "IC_" #name;}

    switch (a_code)
    {
      INPUT_CODES
      default: { return "BAD INPUT"; }
    }
    return "BAD INPUT";
  }

  char const * GetInputEventString(InputEvent a_event)
  {
#undef ITEM
#define ITEM(name) case IE_ ## name: {return "IE_" #name;}

    switch (a_event)
    {
      INPUT_EVENTS
      default: { return "BAD INPUT"; }
    }
    return "BAD INPUT";
  }
}