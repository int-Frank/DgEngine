//@group UI

#ifndef UICOMMON_H
#define UICOMMON_H

#include <stdint.h>
#include "core_utils.h"

namespace Engine
{
  struct UIAABB
  {
    vec2 position;
    vec2 size;
  };

  bool UIInit();
  void UIDestroy();

  bool UIPointInBox(vec2 const & point, UIAABB const & box);
}

#endif