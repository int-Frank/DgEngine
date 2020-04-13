#ifndef GC_TYPES_H
#define GC_TYPES_H

#include <stdint.h>
#include "DgR3Vector.h"
#include "DgR2Vector.h"
#include "DgR3Matrix.h"
#include "DgR2Disk.h"
#include "DgR3Sphere.h"
#include "DgR2AABB.h"
#include "DgR2Segment.h"
#include "DgR2Line.h"
#include "DgR3Rectangle.h"

namespace GC
{
  typedef uint32_t                        IndexType;
  typedef Dg::R3::Vector<float>           vec4;
  typedef Dg::R2::Vector<float>           vec3;
  typedef Dg::R2::Vector_cartesian<float> vec2;
  typedef Dg::R3::Matrix<float>           mat4;

  typedef Dg::R2::Disk<float>             disk2D;
  typedef Dg::R2::AABB<float>             AABB2D;
  typedef Dg::R2::Segment<float>          lineSeg2D;
  typedef Dg::R2::Line<float>             line2D;
  typedef Dg::R3::Sphere<float>           sphere;
  typedef Dg::R3::Rectangle<float>        rect3D;
}

#endif