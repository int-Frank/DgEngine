//@group Renderer/RenderThread

/*
  Original Copyright Yan Chernikov <github.com/TheCherno/Hazel-dev> and contributors.

  The following code is a derivative work of the code from the GitHub project 'Hazel-dev',
  which is licensed under:

                                  Apache License
                             Version 2.0, January 2004
                          http://www.apache.org/licenses

  This code therefore is also licensed under the terms
  of the Apache License Version 2.0

  Copyright 2017-2019 Frank Hart <frankhart010@gmail.com>
*/

#ifndef RT_RENDERERAPI_H
#define RT_RENDERERAPI_H

#include <stdint.h>
#include <string>

#include "RenderCommon.h"

namespace DgE
{
  using RendererID = uint32_t;
#define INVALID_RENDERER_ID 0xFFFFFFFFUL

  struct RenderAPICapabilities
  {
    RenderAPICapabilities();

    std::string vendor;
    std::string renderer;
    std::string version;

    int maxSamples;
    float maxAnisotropy;
    int maxTextureUnits;

    int maxVertexUniformBlocks;
    int maxFragmentUniformBlocks;
    int maxGeometryUniformBlocks;
    int maxUniformBlockSize;
    int maxUniformBufferBindings;

    int maxVertexShaderStorageBlocks;
    int maxFragmentShaderStorageBlocks;
    int maxGeometryShaderStorageBlocks;
    int maxShaderStorageBlockSize;
    int maxShaderStorageBufferBindings;
  };

  class RendererAPI
  {
  private:

  public:
    static void Init();
    static void ShutDown();

    static void Clear();
    static void Clear(float r, float g, float b, float a);
    static void SetClearColor(float r, float g, float b, float a);
    static void SetSissorBox(int x, int y, int w, int h);
    static void Enable(RenderFeature);
    static void Disable(RenderFeature);
    static void DrawIndexed(RenderMode, IndexDataType, uint32_t instanceCount, uint32_t elementCount);

    static void LoadRequiredAssets();

    static RenderAPICapabilities& GetCapabilities()
    {
      static RenderAPICapabilities capabilities;
      return capabilities;
    }

  };
}

#endif