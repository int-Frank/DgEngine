//@group Renderer

#include "RenderCommon.h"

namespace DgE
{
  uint32_t GetIndexDataTypeSize(IndexDataType a_type)
  {
    static uint32_t const s_Size[static_cast<uint32_t>(IndexDataType::COUNT)] =
    {
      1, 2, 4
    };
    return s_Size[static_cast<uint32_t>(a_type)];
  }
}