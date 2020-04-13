//@group Renderer/RenderThread

#ifndef RENDERTHREADDATA_H
#define RENDERTHREADDATA_H

#include "DgOpenHashMap.h"
//#include "RT_RendererAPI.h"
#include "RT_Buffer.h"
#include "RT_VertexArray.h"
#include "RT_RendererProgram.h"
#include "RT_BindingPoint.h"
#include "RT_Texture.h"
#include "ResourceID.h"

namespace Engine
{
  class RenderThreadData
  {
    static RenderThreadData* s_instance;
  public:

    static bool Init();
    static void ShutDown();
    static RenderThreadData* Instance();

  public:

    Dg::OpenHashMap<RefID, RT_VertexArray>          VAOs;
    Dg::OpenHashMap<RefID, RT_IndexBuffer>          IBOs;
    Dg::OpenHashMap<RefID, RT_VertexBuffer>         VBOs;
    Dg::OpenHashMap<RefID, RT_UniformBuffer>        UBOs;
    Dg::OpenHashMap<RefID, RT_ShaderStorageBuffer>  SSBOs;
    Dg::OpenHashMap<RefID, RT_BindingPoint>         bindingPoints;
    Dg::OpenHashMap<RefID, RT_Texture2D>            textures;
    Dg::OpenHashMap<RefID, RT_RendererProgram>      rendererPrograms;
  };
}

#endif