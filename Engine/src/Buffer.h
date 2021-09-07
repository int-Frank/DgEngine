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
#include "Options.h"
#include "RenderResource.h"
#include "ShaderUniform.h"
#include "ShaderUtils.h"
#include "RenderCommon.h"

namespace DgE 
{
  enum class BufferUsage
  {
    None = 0,
    Static = 1,
    Dynamic = 2,
    DynamicCopy = 3,
  };


  enum BufferFlags
  {
    BF_None = 0,
    BF_Mapped = 1
  };


  struct BufferElement
  {
    ShaderDataType type;
    uint32_t size;
    uint32_t offset;
    bool normalized;

    BufferElement() = default;

    size_t Size() const;
    void* Serialize(void*) const;
    void const* Deserialize(void const*);

    BufferElement(ShaderDataType, bool normalized = false);
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

    std::vector<BufferElement> m_elements;
    uint32_t m_stride;
  };

  //------------------------------------------------------------------------------------------------
  // VertexBuffer
  //------------------------------------------------------------------------------------------------

  // TODO Add option to Create buffers using memory allocated from the render heap.
  //      This way we eliminate the need to do any copying.
  class VertexBuffer : public RenderResource
  {
  private:
    VertexBuffer(void const * data, uint32_t size, uint32_t a_flags, BufferUsage a_usage = BufferUsage::Static);
    VertexBuffer(uint32_t size, uint32_t a_flags, BufferUsage a_usage = BufferUsage::Static);

    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer& operator=(VertexBuffer const&) = delete;

  public:
    
    static Ref<VertexBuffer> Create(void const * a_pData,
                                  uint32_t a_size,
                                  uint32_t a_flags,
                                  BufferUsage a_usage = BufferUsage::Static);
    
    static Ref<VertexBuffer> Create(uint32_t a_size,
                                    uint32_t a_flags,
                                    BufferUsage a_usage = BufferUsage::Static);

    ~VertexBuffer();

    // What I'm trying to do here is come up with a way to write to a mapped buffer without
    // having to bubble up the buffer pointer to the main thread.
    typedef void (*MyFunc)(void * pMappedMemory, void * pUserData);
    void WriteToMapped(MyFunc, void *pUserData = nullptr);

    void SetData(void const * data, uint32_t size, uint32_t offset = 0);
    void Bind() const;

    void SetLayout(BufferLayout const &);
  };

  //------------------------------------------------------------------------------------------------
  // UniformBuffer
  //------------------------------------------------------------------------------------------------

  // TODO update constructors with flags
  class UniformBuffer : public RenderResource
  {
  private:
    UniformBuffer(uint32_t size, BufferUsage);
    UniformBuffer(void const * a_pData, uint32_t size, BufferUsage);

    UniformBuffer(UniformBuffer const&) = delete;
    UniformBuffer& operator=(UniformBuffer const&) = delete;

  public:

    static Ref<UniformBuffer> Create(void const * a_pData, uint32_t size, BufferUsage = BufferUsage::Static);
    static Ref<UniformBuffer> Create(uint32_t size, BufferUsage = BufferUsage::Static);

    ~UniformBuffer();

    void Bind(Ref<BindingPoint> const &);

    void SetData(void const * a_pData, uint32_t a_size, uint32_t a_offset);
  };

  //------------------------------------------------------------------------------------------------
  // ShaderStorageBuffer
  //------------------------------------------------------------------------------------------------

  /*class ShaderStorageBuffer : public RenderResource
  {
  private:
    ShaderStorageBuffer(void const * data, uint32_t size, BufferUsage a_usage = BufferUsage::Static);
    ShaderStorageBuffer(uint32_t size, BufferUsage a_usage = BufferUsage::Static);

    ShaderStorageBuffer(ShaderStorageBuffer const&) = delete;
    ShaderStorageBuffer& operator=(ShaderStorageBuffer const&) = delete;

  public:
    
    static Ref<ShaderStorageBuffer> Create(void const * a_pData,
                                           uint32_t a_size,
                                           BufferUsage a_usage = BufferUsage::Static);

    static Ref<ShaderStorageBuffer> Create(uint32_t a_size,
                                           BufferUsage a_usage = BufferUsage::Static);

    ~ShaderStorageBuffer();

    void SetData(void const * data, uint32_t size, uint32_t offset = 0);
    void Bind() const;
    void Bind(Ref<BindingPoint> const&);

    void SetLayout(BufferLayout const &);
  };*/


  //------------------------------------------------------------------------------------------------
  // IndexBuffer
  //------------------------------------------------------------------------------------------------

  // TODO update constructors with flags
  class IndexBuffer : public RenderResource
  {
    IndexBuffer(void const * a_pData, IndexDataType a_dataType, uint32_t a_count);

    IndexBuffer(IndexBuffer const&) = delete;
    IndexBuffer& operator=(IndexBuffer const&) = delete;
  public:

    // TODO add flag to take ownership of the memory
    //      persistant: can just pass pointer to RT_Buffer, but do not free
    //      makeCopy: must copy data
    //      dealloc: can take ownership and must free once done
    static Ref<IndexBuffer> Create(uint8_t const * a_pData, uint32_t a_count);
    static Ref<IndexBuffer> Create(uint16_t const * a_pData, uint32_t a_count);
    static Ref<IndexBuffer> Create(uint32_t const * a_pData, uint32_t a_count);

     ~IndexBuffer();

    uint32_t ElementCount() const;
    IndexDataType DataType() const;

    void SetData(void const * data, uint32_t size, uint32_t offset);
    void Bind() const;

  private:

    IndexDataType m_dataType;
    uint32_t m_elementCount;
  };

}
#endif