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

#ifndef RT_BUFFER
#define RT_BUFFER

#include "Buffer.h"
#include "RT_RendererAPI.h"
#include "RT_BindingPoint.h"

namespace Engine
{
  class RT_BindingPoint;

  enum class BufferType : uint32_t
  {
    None = 0,
    Vertex,
    Index,
    Uniform,
    ShaderStorage
  };

  //------------------------------------------------------------------------------------------------
  // BufferBase
  //------------------------------------------------------------------------------------------------

  class RT_BufferBase
  {
    virtual BufferType GetType() const = 0;

  public:

    RT_BufferBase();
    virtual ~RT_BufferBase();

    void SetData(void* data, uint32_t size, uint32_t offset = 0);
    void Bind() const;

    void Init(void* data, uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
    void Init(uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
    void Destroy();

    BufferLayout const& GetLayout() const;
    void SetLayout(BufferLayout const&);
    uint32_t GetSize() const;
    RendererID GetRendererID() const;

  protected:
    RendererID m_rendererID;
  private:
    uint32_t m_size;
    BufferUsage m_usage;
    BufferLayout m_layout;
  };

  //------------------------------------------------------------------------------------------------
  // Vertex
  //------------------------------------------------------------------------------------------------

  class RT_VertexBuffer : public RT_BufferBase
  {
    BufferType GetType() const;
  };

  //------------------------------------------------------------------------------------------------
  // Indexed Buffer
  //------------------------------------------------------------------------------------------------

  class RT_IndexedBuffer : public RT_BufferBase
  {
    virtual BufferType GetType() const = 0;
    virtual bool CanBind(RT_BindingPoint const&) const = 0;
  public:
    virtual ~RT_IndexedBuffer() {}
    void BindToPoint(RT_BindingPoint const&);
  private:
  };

  //------------------------------------------------------------------------------------------------
  // Uniform Buffer
  //------------------------------------------------------------------------------------------------

  class RT_UniformBuffer : public RT_IndexedBuffer
  {
    BufferType GetType() const;
    bool CanBind(RT_BindingPoint const&) const;
  };

  //------------------------------------------------------------------------------------------------
  // Shader Storage Buffer
  //------------------------------------------------------------------------------------------------

  class RT_ShaderStorageBuffer : public RT_IndexedBuffer
  {
    BufferType GetType() const;
    bool CanBind(RT_BindingPoint const&) const;
  };

  //------------------------------------------------------------------------------------------------
  // IndexBuffer
  //------------------------------------------------------------------------------------------------
  class RT_IndexBuffer
  {
  public:
    typedef ::Engine::IndexBuffer::intType intType;

    RT_IndexBuffer();
    ~RT_IndexBuffer();

    void Init(void* data, uint32_t size);
    void Destroy();

    void SetData(void* data, uint32_t size, uint32_t offset = 0);
    void Bind() const;

    uint32_t GetCount() const;
    uint32_t GetSize() const;
    RendererID GetRendererID() const;

  private:
    RendererID m_rendererID;
    uint32_t m_size;
  };

}

#endif
