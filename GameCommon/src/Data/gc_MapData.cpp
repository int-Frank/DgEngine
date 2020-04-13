#include "gc_MapData.h"

#define DG_ASSERTS_ENABLED
#include "DgAssert.h"

#define GetSubInt64(item, data) Dg::GetSubInt<DataType, static_cast<DataType>(Loc::item), static_cast<DataType>(Size::item)>(data)
#define SetSubInt64(item, data, val) Dg::SetSubInt<DataType, static_cast<DataType>(Loc::item), static_cast<DataType>(Size::item)>(data, val)

namespace GC
{
  BSP_Node::Meta const BSP_Node::s_meta[6] =
  {
    {31, 1},
    {0, 31},
    {0, 12},
    {12, 12},
    {24, 6},
    {30, 1},
  };

  Arc const Corner::Arcs[4] = 
  {
    {{0.0f, 1.0f, 0.0f},  {1.0f, 0.0f, 0.0f}},
    {{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{1.0f, 0.0f, 0.0f},  {0.0f, -1.0f, 0.0f}}
  };

  Wall::Wall()
    : direction(1.0f, 0.0f, 0.0f)
    , origin(0.0f, 0.0f, 1.0f)
    , length(1.0f)
  {

  }

  vec2 Wall::GetNormal() const
  {
    return vec2(direction[1], -direction[0]);
  }

  BSP_Node::BSP_Node()
    : m_data(0)
  {

  }

  BSP_Node::BSP_Node(uint64_t a_data)
    : m_data(a_data)
  {

  }

  
}