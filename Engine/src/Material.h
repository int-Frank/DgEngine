//@group Renderer

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


#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "Memory.h"
#include "MemBuffer.h"
#include "RendererProgram.h"
#include "core_Assert.h"
#include "DgDynamicArray.h"
#include "DgMap_AVL.h"
#include "Texture.h"

namespace Engine
{
  namespace impl
  {
    class MaterialData : public Resource
    {
      MaterialData(Ref<RendererProgram>);
    public:

      static Ref<MaterialData> Create(Ref<RendererProgram>);
      ~MaterialData();

    public:
      Ref<RendererProgram> m_prog;
    };

    class MaterialBase
    {
    public:

      MaterialBase(Ref<impl::MaterialData>);
      virtual ~MaterialBase();
      void Bind();

    protected:

      ShaderUniformDeclaration const * FindUniform(std::string const &);
      UniformBufferElementHeader CreateHeader(ShaderUniformDeclaration const*, uint32_t size);
      void WriteToBuffer(uint32_t offset, UniformBufferElementHeader header, void const * buffer);
      
    protected:

      Ref<impl::MaterialData>               m_materialData;
      uint32_t                              m_bufSize;
      byte*                                 m_pBuf;
    };
  }

  class MaterialInstance : public impl::MaterialBase
  {
    friend class Material;
  public:

    ~MaterialInstance();
    void SetUniform(std::string const& uniform, void const* data, uint32_t size);
    void SetTexture(std::string const& name, Ref<Texture2D> const&);

  private: //Accessed by Material

    MaterialInstance(Ref<impl::MaterialData>);
    void InitBuffer(byte const *);
    void SetUniform(uint32_t offset, void const* data, uint32_t size);

  private:

  };

  class Material : public impl::MaterialBase
  {
    Material(Ref<impl::MaterialData>);
  public:
    ~Material();

    static Ref<Material> Create(Ref<RendererProgram>);
    Ref<MaterialInstance> SpawnInstance();
    void SetUniform(std::string const& name, void const* data, uint32_t size);
    void SetTexture(std::string const& name, Ref<Texture2D> const&);

  private:
    Dg::Map_AVL<std::string, ResourceID>  m_textureBindings;
    Dg::DynamicArray<Ref<MaterialInstance>> m_materialInstances;
    uint32_t m_renderFlags;
  };

}

#endif