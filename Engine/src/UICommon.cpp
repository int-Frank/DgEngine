//@group UI

#include "UICommon.h"
#include "UIWidget.h"

namespace Engine
{
  bool UIPointInBox(vec2 const & a_point, UIAABB const & a_box)
  {
    if (a_point.x() < a_box.position.x())
      return false;

    if (a_point.y() < a_box.position.y())
      return false;

    if (a_point.x() > a_box.position.x() + a_box.size.x())
      return false;

    if (a_point.y() > a_box.position.y() + a_box.size.y())
      return false;

    return true;
  }
}