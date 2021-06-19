#ifndef COMMON_H
#define COMMON_H

#include "System.h"

#include <stdint.h>

uint32_t NextID();

enum SystemID : Engine::SystemID
{
  SID_RenderDemo = Engine::RSID_BEGIN,
  SID_GUIDemo,
};

#endif