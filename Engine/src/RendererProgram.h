//@group Renderer

#ifndef RENDERERPROGRAM_H
#define RENDERERPROGRAM_H

#include "RenderResource.h"
#include "ResourceManager.h"
#include "ShaderUniform.h"
#include "Memory.h"
#include "Utils.h"

namespace DgE
{
  class RendererProgram : public RenderResource
  {
    void Init(ResourceID shaderSourceID);
    RendererProgram();

    RendererProgram(RendererProgram const&) = delete;
    RendererProgram& operator=(RendererProgram const&) = delete;
  public:

    static Ref<RendererProgram> Create(ResourceID shaderSourceID);

    ~RendererProgram();

    uint32_t UniformBufferSize() const;

    void UploadUniformBuffer(byte const *);
    ShaderUniformDeclaration const * FindUniformDeclaration(std::string const&) const;

    //Deprecated
    void UploadUniform(std::string const& name, void const * buf, uint32_t size);

    //Use if buf is already on the renderer memory arena
    //void UploadUniformNoCopy(std::string const& name, void const * buf, uint32_t size);

    void Bind();
    void Unbind();

  private:

    // Now that we have access to the uniform data, we can create a buffer to transform
    // uniforms over to the render thread
    ResourceID m_shaderDataID;
  };
}

#endif