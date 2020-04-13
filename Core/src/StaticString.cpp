#include "StaticString.h"

#undef ITEM
#define ITEM(...) STR_PASTE(HELPER(ITEM_STR, NARGS(__VA_ARGS__)),(__VA_ARGS__))
char const* Str::ToStr[] =
{
  STATIC_STRINGS
};