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
#include "RenderResource.h"

namespace Engine
{
  class RenderThreadData
  {
    static RenderThreadData* s_instance;

    ~RenderThreadData();

  public:

    static bool Init();
    static void ShutDown();
    static RenderThreadData* Instance();

  public:

    Dg::OpenHashMap<RenderResourceID, RT_VertexArray*>          VAOs;
    Dg::OpenHashMap<RenderResourceID, RT_IndexBuffer*>          IBOs;
    Dg::OpenHashMap<RenderResourceID, RT_VertexBuffer*>         VBOs;
    Dg::OpenHashMap<RenderResourceID, RT_UniformBuffer*>        UBOs;
    //Dg::OpenHashMap<RenderResourceID, RT_ShaderStorageBuffer*>  SSBOs;
    Dg::OpenHashMap<RenderResourceID, RT_BindingPoint*>         bindingPoints;
    Dg::OpenHashMap<RenderResourceID, RT_Texture2D*>            textures;
    Dg::OpenHashMap<RenderResourceID, RT_RendererProgram*>      rendererPrograms;
  };
}

#endif