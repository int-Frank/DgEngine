#include "common.h"

uint32_t NextID()
{
  static uint32_t s_ID = 0;
  return ++s_ID;
}