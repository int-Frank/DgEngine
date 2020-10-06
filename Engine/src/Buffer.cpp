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

#pragma once

#include "Buffer.h"
#include "Renderer.h"
#include  "core_Log.h"
#include "RenderThreadData.h"
#include "ShaderUtils.h"
#include "Serialize.h"

namespace Engine 
{
  //------------------------------------------------------------------------------------------------
  // BufferElement
  //------------------------------------------------------------------------------------------------
  BufferElement::BufferElement(ShaderDataType a_type, std::string const& a_name, bool a_normalized)
    : name(a_name)
    , type(a_type)
    , size(SizeOfShaderDataType(type))
    , offset(0)
    , normalized(a_normalized)
  {

  }

  uint32_t BufferElement::GetComponentCount() const
  {
    return ::Engine::GetComponentCount(type);
  }

  size_t BufferElement::Size() const
  {
    return size_t(Core::SerializedSize(name))
         + size_t(Core::SerializedSize(type))
         + size_t(Core::SerializedSize(size))
         + size_t(Core::SerializedSize(offset))
         + size_t(Core::SerializedSize(normalized));
  }

  void* BufferElement::Serialize(void* a_out) const
  {
    void * pBuf = a_out;
    uint32_t type32 = static_cast<uint32_t>(type);
    pBuf = Core::Serialize(pBuf, &name);
    pBuf = Core::Serialize(pBuf, &type32);
    pBuf = Core::Serialize(pBuf, &size);
    pBuf = Core::Serialize(pBuf, &offset);
    pBuf = Core::Serialize(pBuf, &normalized);
    return pBuf;
  }

  void const* BufferElement::Deserialize(void const * a_buf)
  {
    void const * pBuf = a_buf;
    uint32_t type32(0);
    pBuf = Core::Deserialize(pBuf, &name);
    pBuf = Core::Deserialize(pBuf, &type32);
    type = uint_ToShaderDataType(type32);
    pBuf = Core::Deserialize(pBuf, &size);
    pBuf = Core::Deserialize(pBuf, &offset);
    pBuf = Core::Deserialize(pBuf, &normalized);
    return pBuf;
  }

  //------------------------------------------------------------------------------------------------
  // BufferLayout
  //------------------------------------------------------------------------------------------------

  BufferLayout::BufferLayout()
    : m_stride(0)
  {

  }

  BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
    : m_elements(elements)
    , m_stride(0)
  {
    CalculateOffsetsAndStride();
  }

  uint32_t BufferLayout::GetStride() const
  {
    return m_stride;
  }

  std::vector<BufferElement> const& BufferLayout::GetElements() const
  {
    return m_elements;
  }

  std::vector<BufferElement>::iterator BufferLayout::begin()
  {
    return m_elements.begin();
  }

  std::vector<BufferElement>::iterator BufferLayout::end()
  {
    return m_elements.end();
  }

  std::vector<BufferElement>::const_iterator BufferLayout::begin() const
  {
    return m_elements.begin();
  }

  std::vector<BufferElement>::const_iterator BufferLayout::end() const
  {
    return m_elements.end();
  }

  void BufferLayout::CalculateOffsetsAndStride()
  {
    uint32_t offset = 0;
    m_stride = 0;
    for (auto& element : m_elements)
    {
      element.offset = offset;
      offset += element.size;
      m_stride += element.size;
    }
  }
  
  size_t BufferLayout::Size() const
  {
    size_t sze = sizeof(m_stride);
    sze += sizeof(uint32_t);
    for (auto const & item : m_elements)
      sze += item.Size();
    return sze;
  }

  void* BufferLayout::Serialize(void* a_out) const
  {
    uint32_t nElements = static_cast<uint32_t>(m_elements.size());
    void* pBuf = a_out;
    pBuf = Core::Serialize(pBuf, &m_stride);
    pBuf = Core::Serialize(pBuf, &nElements);
    for (auto const& item : m_elements)
      pBuf = item.Serialize(pBuf);
    return pBuf;
  }

  void const* BufferLayout::Deserialize(void const* a_buf)
  {
    uint32_t nElements = 0;
    void const * pBuf = a_buf;
    pBuf = Core::Deserialize(pBuf, &m_stride);
    pBuf = Core::Deserialize(pBuf, &nElements);
    for (uint32_t i = 0; i < nElements; i++)
    {
      BufferElement ele;
      pBuf = ele.Deserialize(pBuf);
      m_elements.push_back(ele);
    }
    return pBuf;
  }

  //------------------------------------------------------------------------------------------------
  // VertexBuffer
  //------------------------------------------------------------------------------------------------
  
  VertexBuffer::VertexBuffer(void * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t * data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
      {
      //TODO all of these RT_* should be obtained through Framework, or throught GraphicsFramework class.
        ::Engine::RT_VertexBuffer vb;
        vb.Init(data, size, usage);
        ::Engine::RenderThreadData::Instance()->VBOs.insert(resID, vb);
      });
  }

  VertexBuffer::VertexBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
      {
        ::Engine::RT_VertexBuffer vb;
        vb.Init(size, usage);
        ::Engine::RenderThreadData::Instance()->VBOs.insert(resID, vb);
      });
  }
  
  VertexBuffer::~VertexBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_VertexBuffer * pVBO =  RenderThreadData::Instance()->VBOs.at(resID);
        if (pVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::~VertexBuffer: RefID '{}' does not exist!", resID);
          return;
        }
        pVBO->Destroy();
        ::Engine::RenderThreadData::Instance()->VBOs.erase(resID);
      });
  }

  void VertexBuffer::SetData(void * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
      {
        ::Engine::RT_VertexBuffer * pVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
        if (pVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::SetData(): RefID '{}' does not exist!", resID);
          return;
        }

        pVBO->SetData(data, size, offset);
      });
  }

  void VertexBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
      {
        ::Engine::RT_VertexBuffer * pVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
        if (pVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::Bind(): RefID '{}' does not exist!", resID);
          return;
        }

        pVBO->Bind();
      });
  }

  void VertexBuffer::SetLayout(BufferLayout const& a_layout)
  {
    void * buffer = RENDER_ALLOCATE(static_cast<uint32_t>(a_layout.Size()));
    a_layout.Serialize(buffer);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetLayout);

    RENDER_SUBMIT(state, [resID = m_id, buffer = buffer]()
    {
      ::Engine::RT_VertexBuffer* pVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
      if (pVBO == nullptr)
      {
        LOG_WARN("VertexBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(buffer);
      pVBO->SetLayout(layout);
    });
  }

  Ref<VertexBuffer> VertexBuffer::Create(void * a_pData,
                                         uint32_t a_size,
                                         BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    return Ref<VertexBuffer>(new VertexBuffer(a_pData, a_size, a_usage));
  }

  Ref<VertexBuffer> VertexBuffer::Create(uint32_t a_size,
                                  BufferUsage a_usage)
  {
    return Ref<VertexBuffer>(new VertexBuffer(a_size, a_usage));
  }

  //------------------------------------------------------------------------------------------------
  // UniformBuffer
  //------------------------------------------------------------------------------------------------

  UniformBuffer::UniformBuffer(void * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
    {
      ::Engine::RT_UniformBuffer ub;
      ub.Init(data, size, usage);
      ::Engine::RenderThreadData::Instance()->UBOs.insert(resID, ub);
    });
  }

  UniformBuffer::UniformBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
    {
      ::Engine::RT_UniformBuffer ub;
      ub.Init(size, usage);
      ::Engine::RenderThreadData::Instance()->UBOs.insert(resID, ub);
    });
  }

  UniformBuffer::~UniformBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
    {
      RT_UniformBuffer* pUBO =  RenderThreadData::Instance()->UBOs.at(resID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::~UniformBuffer: RefID '{}' does not exist!", resID);
        return;
      }
      pUBO->Destroy();
      ::Engine::RenderThreadData::Instance()->UBOs.erase(resID);
    });
  }

  void UniformBuffer::SetData(void * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
    {
      ::Engine::RT_UniformBuffer* pUBO = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::SetData(): RefID '{}' does not exist!", resID);
        return;
      }

      pUBO->SetData(data, size, offset);
    });
  }

  void UniformBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      ::Engine::RT_UniformBuffer* pUBO = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::Bind(): RefID '{}' does not exist!", resID);
        return;
      }

      pUBO->Bind();
    });
  }

  void UniformBuffer::SetLayout(BufferLayout const& a_layout)
  {
    void* buffer = RENDER_ALLOCATE(static_cast<uint32_t>(a_layout.Size()));
    a_layout.Serialize(buffer);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetLayout);

    RENDER_SUBMIT(state, [resID = m_id, buffer = buffer]()
    {
      ::Engine::RT_UniformBuffer* pUBO = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(buffer);
      pUBO->SetLayout(layout);
    });
  }

  Ref<UniformBuffer> UniformBuffer::Create(void * a_pData,
                                           uint32_t a_size,
                                           BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    return Ref<UniformBuffer>(new UniformBuffer(a_pData, a_size, a_usage));
  }

  Ref<UniformBuffer> UniformBuffer::Create(uint32_t a_size,
                                           BufferUsage a_usage)
  {
    return Ref<UniformBuffer>(new UniformBuffer(a_size, a_usage));
  }

  void UniformBuffer::Bind(Ref<BindingPoint> const& a_bp)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::IndexedBufferBind);

    RENDER_SUBMIT(state, [uboID = m_id, bpID = a_bp->GetID()]()
    {
      ::Engine::RT_UniformBuffer* pUBO = ::Engine::RenderThreadData::Instance()->UBOs.at(uboID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): UBO RefID '{}' does not exist!", uboID);
        return;
      }

      ::Engine::RT_BindingPoint* pBP = ::Engine::RenderThreadData::Instance()->bindingPoints.at(bpID);
      if (pUBO == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): BP RefID '{}' does not exist!", bpID);
        return;
      }
      
      pUBO->BindToPoint(*pBP);
    });
  }
  
  //------------------------------------------------------------------------------------------------
  // ShaderStorageBuffer
  //------------------------------------------------------------------------------------------------
  
  ShaderStorageBuffer::ShaderStorageBuffer(void * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
    {
      ::Engine::RT_ShaderStorageBuffer ssb;
      ssb.Init(data, size, usage);
      ::Engine::RenderThreadData::Instance()->SSBOs.insert(resID, ssb);
    });
  }

  ShaderStorageBuffer::ShaderStorageBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
    {
      ::Engine::RT_ShaderStorageBuffer ssb;
      ssb.Init(size, usage);
      ::Engine::RenderThreadData::Instance()->SSBOs.insert(resID, ssb);
    });
  }

  ShaderStorageBuffer::~ShaderStorageBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
    {
      RT_ShaderStorageBuffer* pSSBO =  RenderThreadData::Instance()->SSBOs.at(resID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::~ShaderStorageBuffer: RefID '{}' does not exist!", resID);
        return;
      }
      pSSBO->Destroy();
      ::Engine::RenderThreadData::Instance()->SSBOs.erase(resID);
    });
  }

  void ShaderStorageBuffer::SetData(void * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
    {
      ::Engine::RT_ShaderStorageBuffer* pSSBO = ::Engine::RenderThreadData::Instance()->SSBOs.at(resID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetData(): RefID '{}' does not exist!", resID);
        return;
      }

      pSSBO->SetData(data, size, offset);
    });
  }

  void ShaderStorageBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      ::Engine::RT_ShaderStorageBuffer* pSSBO = ::Engine::RenderThreadData::Instance()->SSBOs.at(resID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::Bind(): RefID '{}' does not exist!", resID);
        return;
      }

      pSSBO->Bind();
    });
  }

  void ShaderStorageBuffer::SetLayout(BufferLayout const& a_layout)
  {
    void* buffer = RENDER_ALLOCATE(static_cast<uint32_t>(a_layout.Size()));
    a_layout.Serialize(buffer);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetLayout);

    RENDER_SUBMIT(state, [resID = m_id, buffer = buffer]()
    {
      ::Engine::RT_ShaderStorageBuffer* pSSBO = ::Engine::RenderThreadData::Instance()->SSBOs.at(resID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(buffer);
      pSSBO->SetLayout(layout);
    });
  }

  Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(void * a_pData,
                                           uint32_t a_size,
                                           BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    return Ref<ShaderStorageBuffer>(new ShaderStorageBuffer(a_pData, a_size, a_usage));
  }

  Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t a_size,
                                           BufferUsage a_usage)
  {
    return Ref<ShaderStorageBuffer>(new ShaderStorageBuffer(a_size, a_usage));
  }

  void ShaderStorageBuffer::Bind(Ref<BindingPoint> const& a_bp)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::IndexedBufferBind);

    RENDER_SUBMIT(state, [uboID = m_id, bpID = a_bp->GetID()]()
    {
      ::Engine::RT_ShaderStorageBuffer* pSSBO = ::Engine::RenderThreadData::Instance()->SSBOs.at(uboID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): SSBO RefID '{}' does not exist!", uboID);
        return;
      }

      ::Engine::RT_BindingPoint* pBP = ::Engine::RenderThreadData::Instance()->bindingPoints.at(bpID);
      if (pSSBO == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): BP RefID '{}' does not exist!", bpID);
        return;
      }
      
      pSSBO->BindToPoint(*pBP);
    });
  }

  //------------------------------------------------------------------------------------------------
  // IndexBuffer
  //------------------------------------------------------------------------------------------------

  IndexBuffer::IndexBuffer(void * a_pData, uint32_t a_size)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, data]()
      {
        ::Engine::RT_IndexBuffer ib;
        ib.Init(data, size);
        ::Engine::RenderThreadData::Instance()->IBOs.insert(resID, ib);
      });
  }

  Ref<IndexBuffer> IndexBuffer::Create(void * a_pData, uint32_t a_size)
  {
    BSR_ASSERT(a_pData != nullptr);

    return Ref<IndexBuffer>(new IndexBuffer(a_pData, a_size));
  }

  IndexBuffer::~IndexBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_IndexBuffer * pIB =  RenderThreadData::Instance()->IBOs.at(resID);
        if (pIB == nullptr)
        {
          LOG_WARN("IndexBuffer::~IndexBuffer: RefID '{}' does not exist!", resID);
          return;
        }
        pIB->Destroy();
        ::Engine::RenderThreadData::Instance()->IBOs.erase(resID);
      });
  }

  void IndexBuffer::SetData(void * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
      {
        ::Engine::RT_IndexBuffer * pIBO = ::Engine::RenderThreadData::Instance()->IBOs.at(resID);
        if (pIBO == nullptr)
        {
          LOG_WARN("IndexBuffer::SetData(): RefID '{}' does not exist!", resID);
          return;
        }

        pIBO->SetData(data, size, offset);
      });
  }

  void IndexBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
      {
        ::Engine::RT_IndexBuffer * pIBO = ::Engine::RenderThreadData::Instance()->IBOs.at(resID);
        if (pIBO == nullptr)
        {
          LOG_WARN("IndexBuffer::Bind(): RefID '{}' does not exist!", resID);
          return;
        }

        pIBO->Bind();
      });
  }
}