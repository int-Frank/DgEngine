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

#ifndef EN_BUFFER_H
#define EN_BUFFER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "Memory.h"
#include "core_Assert.h"
#include "Resource.h"
#include "ShaderUniform.h"
#include "ShaderUtils.h"

namespace Engine 
{
  enum class BufferUsage
  {
    None = 0,
    Static = 1,
    Dynamic = 2,
    DynamicCopy = 3,
  };

  struct BufferElement
  {
    //TODO Do we even need name? Perhaps just to see in the code what it is?
    std::string name;
    ShaderDataType type;
    uint32_t size;
    uint32_t offset;
    bool normalized;

    BufferElement() = default;

    size_t Size() const;
    void* Serialize(void*) const;
    void const* Deserialize(void const*);

    BufferElement(ShaderDataType, std::string const & name, bool normalized = false);
    uint32_t GetComponentCount() const;
  };

  class BufferLayout
  {
  public:
    BufferLayout();

    BufferLayout(std::initializer_list<BufferElement> const & elements);

    uint32_t GetStride() const;
    std::vector<BufferElement> const & GetElements() const;

    std::vector<BufferElement>::iterator begin();
    std::vector<BufferElement>::iterator end();
    std::vector<BufferElement>::const_iterator begin() const;
    std::vector<BufferElement>::const_iterator end() const;

    size_t Size() const;
    void* Serialize(void*) const;
    void const* Deserialize(void const*);

  private:

    void CalculateOffsetsAndStride();

  private:
    //TODO Make this a Dg::DynamicArray, only, I need to implement constructor from
    //     initializer list.
    std::vector<BufferElement> m_elements;
    uint32_t m_stride;
  };

  //------------------------------------------------------------------------------------------------
  // VertexBuffer
  //------------------------------------------------------------------------------------------------

  class VertexBuffer : public Resource
  {
  private:
    void Init(void* data, uint32_t size, BufferUsage a_usage = BufferUsage::Static);
    void Init(uint32_t size, BufferUsage a_usage = BufferUsage::Static);

    VertexBuffer();

    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer& operator=(VertexBuffer const&) = delete;

  public:
    
    static Ref<VertexBuffer> Create(void* a_data,
                                  uint32_t a_size,
                                  BufferUsage a_usage = BufferUsage::Static);

    static Ref<VertexBuffer> Create(uint32_t a_size,
                                    BufferUsage a_usage = BufferUsage::Static);

    ~VertexBuffer();

    void SetData(void* data, uint32_t size, uint32_t offset = 0);
    void Bind() const;

    void SetLayout(BufferLayout const &);
  };

  //------------------------------------------------------------------------------------------------
  // UniformBuffer
  //------------------------------------------------------------------------------------------------

  class UniformBuffer : public Resource
  {
  private:
    void Init(void* data, uint32_t size, BufferUsage a_usage = BufferUsage::Static);
    void Init(uint32_t size, BufferUsage a_usage = BufferUsage::Static);

    UniformBuffer();

    UniformBuffer(UniformBuffer const&) = delete;
    UniformBuffer& operator=(UniformBuffer const&) = delete;

  public:
    
    static Ref<UniformBuffer> Create(void* a_data,
                                     uint32_t a_size,
                                     BufferUsage a_usage = BufferUsage::Static);

    static Ref<UniformBuffer> Create(uint32_t a_size,
                                     BufferUsage a_usage = BufferUsage::Static);

    ~UniformBuffer();

    void SetData(void* data, uint32_t size, uint32_t offset = 0);
    void Bind() const;
    void Bind(Ref<BindingPoint> const&);

    void SetLayout(BufferLayout const &);
  };

  //------------------------------------------------------------------------------------------------
  // ShaderStorageBuffer
  //------------------------------------------------------------------------------------------------

  class ShaderStorageBuffer : public Resource
  {
  private:
    void Init(void* data, uint32_t size, BufferUsage a_usage = BufferUsage::Static);
    void Init(uint32_t size, BufferUsage a_usage = BufferUsage::Static);

    ShaderStorageBuffer();

    ShaderStorageBuffer(ShaderStorageBuffer const&) = delete;
    ShaderStorageBuffer& operator=(ShaderStorageBuffer const&) = delete;

  public:
    
    static Ref<ShaderStorageBuffer> Create(void* a_data,
                                           uint32_t a_size,
                                           BufferUsage a_usage = BufferUsage::Static);

    static Ref<ShaderStorageBuffer> Create(uint32_t a_size,
                                           BufferUsage a_usage = BufferUsage::Static);

    ~ShaderStorageBuffer();

    void SetData(void* data, uint32_t size, uint32_t offset = 0);
    void Bind() const;
    void Bind(Ref<BindingPoint> const&);

    void SetLayout(BufferLayout const &);
  };


  //------------------------------------------------------------------------------------------------
  // IndexBuffer
  //------------------------------------------------------------------------------------------------

  class IndexBuffer : public Resource
  {
    void Init(void* data, uint32_t size);
    IndexBuffer();

    IndexBuffer(IndexBuffer const&) = delete;
    IndexBuffer& operator=(IndexBuffer const&) = delete;
  public:
    typedef uint16_t intType;

    static Ref<IndexBuffer> Create(void* a_data, uint32_t a_size);

     ~IndexBuffer();

    void SetData(void* data, uint32_t size, uint32_t offset);
    void Bind() const;
  };

}
#endif