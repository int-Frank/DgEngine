//@group UI

#include "UICommon.h"
#include "UIWidget.h"

namespace Engine
{
  bool UIPointInBox(vec2 const & boxMin, vec2 const & boxSize, vec2 const & point)
  {
    if (point.x() < boxMin.x())
      return false;

    if (point.y() < boxMin.y())
      return false;

    if (point.x() > boxMin.x() + boxSize.x())
      return false;

    if (point.y() > boxMin.y() + boxSize.y())
      return false;

    return true;
  }
}