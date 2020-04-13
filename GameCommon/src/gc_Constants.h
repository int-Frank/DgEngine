#ifndef GC_CONSTATNTS_H
#define GC_CONSTATNTS_H

#include "gc_Types.h"
#include "DgEndian.h"

#define MAX_MAPS 100

#define MAPHEADER "MAPHEAD.BS6"
#define MAPTEMP "MAPTEMP.BS6"

#define VEC_NORTH vec3(0.0f, 1.0f, 0.0f)
#define VEC_EAST  vec3(1.0f, 0.0f, 0.0f)
#define VEC_SOUTH vec3(0.0f, -1.0f, 0.0f)
#define VEC_WEST  vec3(-1.0f, 0.0f, 0.0f)

namespace GC
{
  enum Direction : uint32_t
  {
    E_North = 0,
    E_South = 1,
    E_East  = 2,
    E_West  = 3
  };

  namespace Global
  {
    vec3 const NSEW[4] = {VEC_NORTH, VEC_SOUTH, VEC_EAST, VEC_WEST};
  }

  static Dg::Endian const BS_FileEndian = Dg::Endian::Little;
}

#endif