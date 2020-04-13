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
#include <stdint.h>
#include "DgDynamicArray.h"
#include "core_Assert.h"
#include "Memory.h"
#include "Resource.h"
#include "ShaderUtils.h"
#include "ShaderSource.h"

#define BIT(x) (1 << x)
#define INVALID_INDEX -1
namespace Engine
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

  //A data type of STRUCT will just be padding. This can be used 
  //to pad out the front and back of a struct.
  class std140ItemDeclaration
  {
  public:

    std140ItemDeclaration(ShaderDataType, uint32_t count, MatrixLayout layout = MatrixLayout::ColumnMajor);
    void * CopyToBuffer(void * buffer, void const * data) const;

    friend bool operator==(std140ItemDeclaration const&, std140ItemDeclaration const&);
    friend bool operator!=(std140ItemDeclaration const&, std140ItemDeclaration const&);

    void SetBaseAlignment(uint32_t offset);
    void SetFrontPadding(uint32_t padding);

    ShaderDataType Type() const;

    uint32_t Stride() const;
    uint32_t Count() const;
    uint32_t FrontPadding() const;

  private:

    ShaderDataType  const m_type;  //Cannot be struct
    uint32_t        const m_count;
    MatrixLayout    const m_matLayout;

    uint32_t              m_frontPadding;
  };

  typedef Dg::DynamicArray<std140ItemDeclaration> std140UniformBlockList;

  //Matrices will be column-major
  class std140UniformBlock
  {
  public:

    std140UniformBlock(MatrixLayout);
    ~std140UniformBlock();

    friend bool operator==(std140UniformBlock const &, std140UniformBlock const&);

    void Push(std140ItemDeclaration const&);
    void * OutputNext(void *);

    uint32_t Size() const;
    uint32_t ItemCount() const;

  private:
    std::string m_name;
    MatrixLayout m_matrixLayout;
    std140UniformBlockList m_items;
  };

  class std140UniformBlockBuffer
  {
  public:

    std140UniformBlockBuffer();
    ~std140UniformBlockBuffer();

    void Push(std140UniformBlock const&);
    std140UniformBlock * Get(std::string const & name);

    uint32_t Size();

  private:
    Dg::DynamicArray<std140UniformBlock *> m_blocks;
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

  typedef Dg::DynamicArray<ShaderUniformDeclaration> ShaderUniformList;

  class ShaderData : public Resource
  {
  public:

    ShaderData();
    ShaderData(std::initializer_list<ShaderSourceElement> const&);

    static Ref<ShaderData> Create(std::initializer_list<ShaderSourceElement> const&);

    void Init(std::initializer_list<ShaderSourceElement> const&);

    void Clear();
    ShaderUniformDeclaration* FindUniform(std::string const&);
    uint32_t FindUniformIndex(std::string const&);

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

  class BindingPoint : public Resource
  {
    void Init(StorageBlockType, ShaderDomain);
    BindingPoint();
  public:

    static Ref<BindingPoint> Create(StorageBlockType, ShaderDomain);

    ~BindingPoint();
  };
}

#endif