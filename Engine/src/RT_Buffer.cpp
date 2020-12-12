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

#include <glad/glad.h>
#include "BSR_Assert.h"
#include "RT_Buffer.h"
#include "RT_RendererAPI.h"
#include "RT_BindingPoint.h"

namespace Engine
{
  //------------------------------------------------------------------------------------------------
  // Helper functions
  //------------------------------------------------------------------------------------------------

  static GLenum OpenGLUsage(BufferUsage a_usage)
  {
    switch (a_usage)
    {
      case BufferUsage::Static:    return GL_STATIC_DRAW;
      case BufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
    }
    BSR_ASSERT(false, "Unknown vertex buffer usage");
    return 0;
  }

  static GLenum OpenGLBufferType(BufferType a_type)
  {
    switch (a_type)
    {
      case BufferType::Vertex:        return GL_ARRAY_BUFFER;
      case BufferType::Index:         return GL_ELEMENT_ARRAY_BUFFER;
      case BufferType::Uniform:       return GL_UNIFORM_BUFFER;
      case BufferType::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
    }
    BSR_ASSERT(false, "Unknown buffer type");
    return 0;
  }

  //------------------------------------------------------------------------------------------------
  // BufferBase
  //------------------------------------------------------------------------------------------------

  RT_BufferBase::RT_BufferBase(void * a_data, uint32_t a_size, BufferUsage a_usage)
    : m_size(a_size)
    , m_usage(a_usage)
    , m_rendererID(0)
  {
    glCreateBuffers(1, &m_rendererID);
    glNamedBufferData(m_rendererID, m_size, a_data, OpenGLUsage(m_usage));
  }

  RT_BufferBase::RT_BufferBase(uint32_t a_size, BufferUsage a_usage)
    : m_size(a_size)
    , m_usage(a_usage)
    , m_rendererID(0)
  {
    glCreateBuffers(1, &m_rendererID);
    glNamedBufferData(m_rendererID, m_size, nullptr, OpenGLUsage(m_usage));
  }

  RT_BufferBase::~RT_BufferBase()
  {
    glDeleteBuffers(1, &m_rendererID);
  }

  void RT_BufferBase::SetData(void* a_data, uint32_t a_size, uint32_t a_offset)
  {
    glNamedBufferSubData(m_rendererID, a_offset, a_size, a_data);
  }

  void RT_BufferBase::Bind() const
  {
    glBindBuffer(OpenGLBufferType(GetType()), m_rendererID);
  }

  BufferLayout const& RT_BufferBase::GetLayout() const
  {
    return m_layout;
  }

  void RT_BufferBase::SetLayout(BufferLayout const& a_layout)
  {
    m_layout = a_layout;
  }

  uint32_t RT_BufferBase::GetSize() const
  {
    return m_size;
  }

  RendererID RT_BufferBase::GetRendererID() const
  {
    return m_rendererID;
  }

  //------------------------------------------------------------------------------------------------
  // Indexed Buffer
  //------------------------------------------------------------------------------------------------

  RT_IndexedBuffer::RT_IndexedBuffer(void * a_data, uint32_t a_size, BufferUsage a_usage)
    : RT_BufferBase(a_data, a_size, a_usage)
  {
  
  }

  RT_IndexedBuffer::RT_IndexedBuffer(uint32_t a_size, BufferUsage a_usage)
    : RT_BufferBase(a_size, a_usage)
  {

  }

  //------------------------------------------------------------------------------------------------
  // Vertex Buffer
  //------------------------------------------------------------------------------------------------

  RT_VertexBuffer::RT_VertexBuffer(void * a_data, uint32_t a_size, BufferUsage a_usage)
    : RT_BufferBase(a_data, a_size, a_usage)
  {
  
  }

  RT_VertexBuffer::RT_VertexBuffer(uint32_t a_size, BufferUsage a_usage)
    : RT_BufferBase(a_size, a_usage)
  {

  }

  BufferType RT_VertexBuffer::GetType() const
  {
    return BufferType::Vertex;
  }

  RT_VertexBuffer * RT_VertexBuffer::Create(void * a_data, uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_VertexBuffer(a_data, a_size, a_usage);
  }

  RT_VertexBuffer * RT_VertexBuffer::Create(uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_VertexBuffer(a_size, a_usage);
  }

  //------------------------------------------------------------------------------------------------
  // Bindable Buffer
  //------------------------------------------------------------------------------------------------
  
  void RT_IndexedBuffer::BindToPoint(RT_BindingPoint const & a_bp)
  {
    BSR_ASSERT(!CanBind(a_bp), "Incorrect buffer type / binding point matchup!");

    //TODO do we need to bind the buffer first?
    glBindBufferBase(OpenGLBufferType(GetType()), a_bp.GetID().Address(), m_rendererID);
  }

  //------------------------------------------------------------------------------------------------
  // Uniform Buffer
  //------------------------------------------------------------------------------------------------
  
  BufferType RT_UniformBuffer::GetType() const
  {
    return BufferType::Uniform;
  }

  bool RT_UniformBuffer::CanBind(RT_BindingPoint const& a_bp) const
  {
    return a_bp.GetID().Type() == StorageBlockType::Uniform;
  }

  RT_UniformBuffer::RT_UniformBuffer(void * a_data, uint32_t a_size, BufferUsage a_usage)
    : RT_IndexedBuffer(a_data, a_size, a_usage)
  {

  }

  RT_UniformBuffer::RT_UniformBuffer(uint32_t a_size, BufferUsage a_usage)
    : RT_IndexedBuffer(a_size, a_usage)
  {

  }

  RT_UniformBuffer * RT_UniformBuffer::Create(void * a_data, uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_UniformBuffer(a_data, a_size, a_usage);
  }

  RT_UniformBuffer * RT_UniformBuffer::Create(uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_UniformBuffer(a_size, a_usage);
  }

  //------------------------------------------------------------------------------------------------
  // Shader storage Buffer
  //------------------------------------------------------------------------------------------------

  BufferType RT_ShaderStorageBuffer::GetType() const
  {
    return BufferType::ShaderStorage;
  }

  bool RT_ShaderStorageBuffer::CanBind(RT_BindingPoint const& a_bp) const
  {
    return a_bp.GetID().Type() == StorageBlockType::ShaderStorage;
  }

  RT_ShaderStorageBuffer::RT_ShaderStorageBuffer(void * a_data, uint32_t a_size, BufferUsage a_usage)
    : RT_IndexedBuffer(a_data, a_size, a_usage)
  {

  }

  RT_ShaderStorageBuffer::RT_ShaderStorageBuffer(uint32_t a_size, BufferUsage a_usage)
    : RT_IndexedBuffer(a_size, a_usage)
  {

  }

  RT_ShaderStorageBuffer * RT_ShaderStorageBuffer::Create(void * a_data, uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_ShaderStorageBuffer(a_data, a_size, a_usage);
  }

  RT_ShaderStorageBuffer * RT_ShaderStorageBuffer::Create(uint32_t a_size, BufferUsage a_usage)
  {
    return new RT_ShaderStorageBuffer(a_size, a_usage);
  }

  //------------------------------------------------------------------------------------------------
  // RT_IndexBuffer
  //------------------------------------------------------------------------------------------------
    
  RT_IndexBuffer::RT_IndexBuffer(void * a_data, uint32_t a_size)
    : m_rendererID(0)
    , m_size(a_size)
  {
    glCreateBuffers(1, &m_rendererID);
    glNamedBufferData(m_rendererID, m_size, a_data, GL_STATIC_DRAW);
  }

  RT_IndexBuffer * RT_IndexBuffer::Create(void* a_data, uint32_t a_size)
  {
    return new RT_IndexBuffer(a_data, a_size);
  }

  RT_IndexBuffer::~RT_IndexBuffer()
  {
    glDeleteBuffers(1, &m_rendererID);
  }

  void RT_IndexBuffer::SetData(void* a_data, uint32_t a_size, uint32_t a_offset)
  {
    glNamedBufferSubData(m_rendererID, a_offset, a_size, a_data);
  }

  void RT_IndexBuffer::Bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
  }

  uint32_t RT_IndexBuffer::GetCount() const
  {
    return m_size / sizeof(intType);
  }

  uint32_t RT_IndexBuffer::GetSize() const
  {
    return m_size;
  }

  RendererID RT_IndexBuffer::GetRendererID() const
  {
    return m_rendererID;
  }
}