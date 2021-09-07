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

#ifndef SHADERUNIFORM_H
#define SHADERUNIFORM_H

#include <string>
#include <vector>
#include <stdint.h>

#include "Options.h"
#include "Memory.h"
#include "RenderResource.h"
#include "ShaderUtils.h"
#include "ShaderSource.h"
#include "DgMap_AVL.h"

#define BIT(x) (1 << x)
#define INVALID_INDEX -1
namespace DgE
{
  class UniformBufferElementHeader
  {
  public:

    enum Flag : uint32_t
    {
      ElementLocked = 0,
      IsInvalid,
      Unused_2,
      Unused_3,
      Unused_4,
      Unused_5,
      Unused_6,
      Unused_7
    };

    UniformBufferElementHeader();

    size_t SerializedSize() const;
    void * Serialize(void *) const;
    void const * Deserialize(void const *);

    void SetSize(uint32_t);
    void SetFlag(Flag, bool);

    uint32_t GetSize() const;
    bool Is(Flag) const;

  private:

    uint32_t m_data;
  };

  class ShaderUniformDeclaration
  {
  public:

    //DEBUG
    void Log() const;

    ShaderUniformDeclaration(ShaderDataType, std::string name, bool isArray, uint32_t count);

    friend bool operator==(ShaderUniformDeclaration const&, ShaderUniformDeclaration const&);

    std::string GetName() const;
    uint32_t GetCount() const;
    ShaderDomains & GetDomains();
    ShaderDataType GetType() const;
    bool IsArray() const;

    uint32_t GetDataSize() const;
    void SetDataOffset(uint32_t offset);
    uint32_t GetDataOffset() const;

  private:

    uint32_t m_dataOffset;

    bool m_isArray;
    std::string m_name;
    uint32_t m_count;
    ShaderDomains m_domains;
    ShaderDataType m_type;
  };

  typedef std::vector<ShaderUniformDeclaration> ShaderUniformList;

  class ShaderData : public RenderResource
  {
    ShaderData();

  public:

    ShaderData(std::initializer_list<ShaderSourceElement> const &);
    static Ref<ShaderData> Create(std::initializer_list<ShaderSourceElement> const &);

    void Init(std::initializer_list<ShaderSourceElement> const&);

    void Clear();
    ShaderUniformDeclaration* FindUniform(std::string const&);
    uint32_t FindUniformIndex(std::string const&) const;

    uint32_t GetUniformDataSize() const;
    ShaderSource const & GetShaderSource() const;
    ShaderUniformList const & GetUniforms() const;
    ShaderUniformList & GetUniforms();
    ShaderUniformList const & GetTextures() const;
    ShaderUniformList & GetTextures();

    //DEBUG
    void Log();

  private:

    struct ShaderStructList;

    void Parse();
    void PostProcess();
    void ExtractStructs(ShaderDomain, ShaderStructList &);
    void ExtractUniforms(ShaderDomain, ShaderStructList const &);
    static size_t FindStruct(std::string const &, ShaderStructList const &);
    void PushUniform(ShaderUniformDeclaration);
  private:
    uint32_t            m_dataSize;

    ShaderSource        m_source;
    ShaderUniformList   m_uniforms;
    ShaderUniformList   m_textures;
  };

  class BindingPoint : public RenderResource
  {
    BindingPoint(StorageBlockType, ShaderDomain);
  public:

    static Ref<BindingPoint> Create(StorageBlockType, ShaderDomain);

    ~BindingPoint();
  };
}

#endif