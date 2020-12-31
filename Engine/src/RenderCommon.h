//@group Renderer

#ifndef RENDERCOMMON_H
#define RENDERCOMMON_H

#include <stdint.h>

namespace Engine
{
  enum class RenderFeature
  {
    Sissor,
    DepthTest,

    COUNT
  };

  enum class RenderMode
  {
    Points,
    Lines,
    Triangles,

    COUNT
  };

  enum class IndexDataType
  {
    unsigned_8,
    unsigned_16,
    unsigned_32,

    COUNT
  };

  uint32_t GetIndexDataTypeSize(IndexDataType);
}

#endif