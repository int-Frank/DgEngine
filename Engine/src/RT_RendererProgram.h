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

#ifndef RT_RENDERERPROGRAM_H
#define RT_RENDERERPROGRAM_H

#include <stdint.h>
#include <vector>
#include "MemBuffer.h"
#include "Utils.h"
#include "ShaderUniform.h"
#include "RT_RendererAPI.h"
#include "ShaderSource.h"
#include "ResourceManager.h"

namespace DgE
{
  class RT_BindingPoint;

  class RT_RendererProgram
  {
    typedef uint32_t Index;

    //RT_RendererProgram();
    RT_RendererProgram(ResourceID shaderDataID);

  public:

    ~RT_RendererProgram();

    static RT_RendererProgram * Create(ResourceID shaderDataID);

    void Bind() const;
    void Unbind() const;

    //Deprecated
    void UploadUniform(std::string const& name, void const* data, uint32_t size);

    /* Each entry in the buffer will be preceded with header, containing data
       such as number of elements to upload and a series of flags.
    */
    void UploadUniformBuffer(byte const* data);

  private:

    bool CompileAndUploadShader();
    void ResolveUniforms();

    //bool Bind(ShaderDomain, std::string const & name, RT_BindingPoint const &);

    int32_t GetUniformLocation(std::string const& name) const;
    void UploadUniform(uint32_t index, void const * buf, uint32_t count);
    void UploadTexture(uint32_t textureUnit, RenderResourceID const * textureIDs, uint32_t count);
    void UploadUniformSingle(int location, ShaderDataType, void const* buf);
    void UploadUniformArray(int location, ShaderDataType, void const* buf, uint32_t count);

  private:

    RendererID m_rendererID;

    std::string m_name;
    ShaderData const * m_pShaderData;
    std::vector<int32_t> m_uniformLocations;
  };
}

#endif