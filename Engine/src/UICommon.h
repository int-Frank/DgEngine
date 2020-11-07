//@group UI

#ifndef UICOMMON_H
#define UICOMMON_H

#include <stdint.h>

#include "DgMatrix.h"

namespace Engine
{
  typedef Dg::Matrix<2, 1, int> vec2i;

  typedef uint64_t SubBlockID;
#define INVALID_UI_TEXTURE 0xFFFF'FFFF'FFFF'FFFF

#define UI_TEXTURE_ID_XY_BITS 20
#define UI_TEXTURE_ID_HW_BITS 12

#define UI_TEXTURE_ID_X(id) static_cast<uint32_t>(id & 0xFFFFFull)
#define UI_TEXTURE_ID_Y(id) static_cast<uint32_t>((id >> UI_TEXTURE_ID_XY_BITS) & 0xFFFFFull)
#define UI_TEXTURE_ID_W(id) static_cast<uint32_t>((id >> (UI_TEXTURE_ID_XY_BITS * 2)) & 0xFFFull)
#define UI_TEXTURE_ID_H(id) static_cast<uint32_t>((id >> (UI_TEXTURE_ID_XY_BITS * 2 + UI_TEXTURE_ID_HW_BITS)) & 0xFFFull)
#define UI_TEXTURE_ID_CREATE(x, y, w, h) (static_cast<uint64_t>(x)\
  | (static_cast<uint64_t>(y) << UI_TEXTURE_ID_XY_BITS)\
  | (static_cast<uint64_t>(w) << (UI_TEXTURE_ID_XY_BITS * 2))\
  | (static_cast<uint64_t>(h) << (UI_TEXTURE_ID_XY_BITS * 2 + UI_TEXTURE_ID_HW_BITS)))
}

#endif