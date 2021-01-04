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
#include  "Log.h"
#include "RenderThreadData.h"
#include "ShaderUtils.h"
#include "Serialize.h"

namespace Engine 
{
  //------------------------------------------------------------------------------------------------
  // BufferElement
  //------------------------------------------------------------------------------------------------
  BufferElement::BufferElement(ShaderDataType a_type, bool a_normalized)
    : type(a_type)
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
    return size_t(SerializedSize(type))
         + size_t(SerializedSize(size))
         + size_t(SerializedSize(offset))
         + size_t(SerializedSize(normalized));
  }

  void* BufferElement::Serialize(void* a_out) const
  {
    void * pBuf = a_out;
    uint32_t type32 = static_cast<uint32_t>(type);
    pBuf = ::Engine::Serialize(pBuf, &type32);
    pBuf = ::Engine::Serialize(pBuf, &size);
    pBuf = ::Engine::Serialize(pBuf, &offset);
    pBuf = ::Engine::Serialize(pBuf, &normalized);
    return pBuf;
  }

  void const* BufferElement::Deserialize(void const * a_buf)
  {
    void const * pBuf = a_buf;
    uint32_t type32(0);
    pBuf = ::Engine::Deserialize(pBuf, &type32);
    type = uint_ToShaderDataType(type32);
    pBuf = ::Engine::Deserialize(pBuf, &size);
    pBuf = ::Engine::Deserialize(pBuf, &offset);
    pBuf = ::Engine::Deserialize(pBuf, &normalized);
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
    pBuf = ::Engine::Serialize(pBuf, &m_stride);
    pBuf = ::Engine::Serialize(pBuf, &nElements);
    for (auto const& item : m_elements)
      pBuf = item.Serialize(pBuf);
    return pBuf;
  }

  void const* BufferLayout::Deserialize(void const* a_buf)
  {
    uint32_t nElements = 0;
    void const * pBuf = a_buf;
    pBuf = ::Engine::Deserialize(pBuf, &m_stride);
    pBuf = ::Engine::Deserialize(pBuf, &nElements);
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
  
  VertexBuffer::VertexBuffer(void const * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t * data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
      {
        ::Engine::RT_VertexBuffer * pVB = ::Engine::RT_VertexBuffer::Create(data, size, usage);
        if (pVB == nullptr)
        {
          LOG_WARN("VertexBuffer::VertexBuffer(): Failed to create vertex buffer!");
          return;
        }
        ::Engine::RenderThreadData::Instance()->VBOs.insert(resID, pVB);
      });
  }

  VertexBuffer::VertexBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
      {
        ::Engine::RT_VertexBuffer * pVB = ::Engine::RT_VertexBuffer::Create(size, usage);
        if (pVB == nullptr)
        {
          LOG_WARN("VertexBuffer::VertexBuffer(): Failed to create vertex buffer!");
          return;
        }
        ::Engine::RenderThreadData::Instance()->VBOs.insert(resID, pVB);
      });
  }
  
  VertexBuffer::~VertexBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_VertexBuffer ** ppVBO =  RenderThreadData::Instance()->VBOs.at(resID);
        if (ppVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::~VertexBuffer: RefID '{}' does not exist!", resID);
          return;
        }
        delete *ppVBO;
        *ppVBO = nullptr;
        ::Engine::RenderThreadData::Instance()->VBOs.erase(resID);
      });
  }

  void VertexBuffer::SetData(void const * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
      {
        ::Engine::RT_VertexBuffer ** ppVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
        if (ppVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::SetData(): RefID '{}' does not exist!", resID);
          return;
        }

        (*ppVBO)->SetData(data, size, offset);
      });
  }

  void VertexBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
      {
        ::Engine::RT_VertexBuffer ** ppVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
        if (ppVBO == nullptr)
        {
          LOG_WARN("VertexBuffer::Bind(): RefID '{}' does not exist!", resID);
          return;
        }

        (*ppVBO)->Bind();
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
      ::Engine::RT_VertexBuffer ** ppVBO = ::Engine::RenderThreadData::Instance()->VBOs.at(resID);
      if (ppVBO == nullptr)
      {
        LOG_WARN("VertexBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(buffer);
      (*ppVBO)->SetLayout(layout);
    });
  }

  Ref<VertexBuffer> VertexBuffer::Create(void const * a_pData,
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

  UniformBuffer::UniformBuffer(void const * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
    {
      ::Engine::RT_UniformBuffer * pUB = ::Engine::RT_UniformBuffer::Create(data, size, usage);
      if (pUB == nullptr)
      {
        LOG_WARN("UniformBuffer::UniformBuffer(): Failed to create uniform buffer!");
        return;
      }
      ::Engine::RenderThreadData::Instance()->UBOs.insert(resID, pUB);
    });
  }

  UniformBuffer::UniformBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
    {
      RT_UniformBuffer * pUB = RT_UniformBuffer::Create(size, usage);
      if (pUB == nullptr)
      {
        LOG_WARN("UniformBuffer::UniformBuffer(): Failed to create uniform buffer!");
        return;
      }
      RenderThreadData::Instance()->UBOs.insert(resID, pUB);
    });
  }

  UniformBuffer::~UniformBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
    {
      RT_UniformBuffer ** ppUB =  RenderThreadData::Instance()->UBOs.at(resID);
      if (ppUB == nullptr)
      {
        LOG_WARN("UniformBuffer::~UniformBuffer: RefID '{}' does not exist!", resID);
        return;
      }
      delete *ppUB;
      *ppUB = nullptr;
      ::Engine::RenderThreadData::Instance()->UBOs.erase(resID);
    });
  }

  void UniformBuffer::SetData(void const * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
    {
      ::Engine::RT_UniformBuffer ** ppUB = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (ppUB == nullptr)
      {
        LOG_WARN("UniformBuffer::SetData(): RefID '{}' does not exist!", resID);
        return;
      }

      (*ppUB)->SetData(data, size, offset);
    });
  }

  void UniformBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      ::Engine::RT_UniformBuffer ** ppUB = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (ppUB == nullptr)
      {
        LOG_WARN("UniformBuffer::Bind(): RefID '{}' does not exist!", resID);
        return;
      }

      (*ppUB)->Bind();
    });
  }

  void UniformBuffer::SetLayout(BufferLayout const& a_layout)
  {
    void* pBuf = RENDER_ALLOCATE(static_cast<uint32_t>(a_layout.Size()));
    a_layout.Serialize(pBuf);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetLayout);

    RENDER_SUBMIT(state, [resID = m_id, pBuf = pBuf]()
    {
      ::Engine::RT_UniformBuffer ** ppUB = ::Engine::RenderThreadData::Instance()->UBOs.at(resID);
      if (ppUB == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(pBuf);
      (*ppUB)->SetLayout(layout);
    });
  }

  Ref<UniformBuffer> UniformBuffer::Create(void const * a_pData,
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
      ::Engine::RT_UniformBuffer ** ppUB = ::Engine::RenderThreadData::Instance()->UBOs.at(uboID);
      if (ppUB == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): UBO RefID '{}' does not exist!", uboID);
        return;
      }

      ::Engine::RT_BindingPoint ** ppBP = ::Engine::RenderThreadData::Instance()->bindingPoints.at(bpID);
      if (ppBP == nullptr)
      {
        LOG_WARN("UniformBuffer::SetLayout(): BP RefID '{}' does not exist!", bpID);
        return;
      }
      
      (*ppUB)->BindToPoint(**ppBP);
    });
  }
  
  //------------------------------------------------------------------------------------------------
  // ShaderStorageBuffer
  //------------------------------------------------------------------------------------------------
  
  ShaderStorageBuffer::ShaderStorageBuffer(void const * a_pData, uint32_t a_size, BufferUsage a_usage)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage, data]()
    {
      RT_ShaderStorageBuffer * pSSB = RT_ShaderStorageBuffer::Create(data, size, usage);
      if (pSSB == nullptr)
      {
        LOG_WARN("RT_ShaderStorageBuffer::RT_ShaderStorageBuffer(): Failed to create object!");
      }
      RenderThreadData::Instance()->SSBOs.insert(resID, pSSB);
    });
  }

  ShaderStorageBuffer::ShaderStorageBuffer(uint32_t a_size, BufferUsage a_usage)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, usage = a_usage]()
    {
      RT_ShaderStorageBuffer * pSSB = RT_ShaderStorageBuffer::Create(size, usage);
      if (pSSB == nullptr)
      {
        LOG_WARN("RT_ShaderStorageBuffer::RT_ShaderStorageBuffer(): Failed to create object!");
      }
      RenderThreadData::Instance()->SSBOs.insert(resID, pSSB);
    });
  }

  ShaderStorageBuffer::~ShaderStorageBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
    {
      RT_ShaderStorageBuffer ** ppSSB =  RenderThreadData::Instance()->SSBOs.at(resID);
      if (ppSSB == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::~ShaderStorageBuffer: RefID '{}' does not exist!", resID);
        return;
      }
      delete * ppSSB;
      *ppSSB = nullptr;
      RenderThreadData::Instance()->SSBOs.erase(resID);
    });
  }

  void ShaderStorageBuffer::SetData(void const * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
    {
      RT_ShaderStorageBuffer ** ppSSB = RenderThreadData::Instance()->SSBOs.at(resID);
      if (ppSSB == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetData(): RefID '{}' does not exist!", resID);
        return;
      }

      (*ppSSB)->SetData(data, size, offset);
    });
  }

  void ShaderStorageBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_ShaderStorageBuffer ** ppSSB = RenderThreadData::Instance()->SSBOs.at(resID);
      if (ppSSB == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::Bind(): RefID '{}' does not exist!", resID);
        return;
      }

      (*ppSSB)->Bind();
    });
  }

  void ShaderStorageBuffer::SetLayout(BufferLayout const& a_layout)
  {
    void* pBuf = RENDER_ALLOCATE(static_cast<uint32_t>(a_layout.Size()));
    a_layout.Serialize(pBuf);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetLayout);

    RENDER_SUBMIT(state, [resID = m_id, pBuf = pBuf]()
    {
      RT_ShaderStorageBuffer ** ppSSB = RenderThreadData::Instance()->SSBOs.at(resID);
      if (ppSSB == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): RefID '{}' does not exist!", resID);
        return;
      }
      BufferLayout layout;
      layout.Deserialize(pBuf);
      (*ppSSB)->SetLayout(layout);
    });
  }

  Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(void const * a_pData,
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
      RT_ShaderStorageBuffer ** ppSSB = RenderThreadData::Instance()->SSBOs.at(uboID);
      if (ppSSB == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): SSBO RefID '{}' does not exist!", uboID);
        return;
      }

      ::Engine::RT_BindingPoint ** ppBP = ::Engine::RenderThreadData::Instance()->bindingPoints.at(bpID);
      if (ppBP == nullptr)
      {
        LOG_WARN("ShaderStorageBuffer::SetLayout(): BP RefID '{}' does not exist!", bpID);
        return;
      }
      
      (*ppSSB)->BindToPoint(**ppBP);
    });
  }

  //------------------------------------------------------------------------------------------------
  // IndexBuffer
  //------------------------------------------------------------------------------------------------

  IndexBuffer::IndexBuffer(void const * a_pData, IndexDataType a_dataType, uint32_t a_count)
    : m_dataType(a_dataType)
    , m_elementCount(a_count)
  {
    BSR_ASSERT(a_pData != nullptr);
    uint32_t dataSize = GetIndexDataTypeSize(m_dataType) * a_count;
    uint8_t* pData = (uint8_t*)RENDER_ALLOCATE(dataSize);
    memcpy(pData, a_pData, dataSize);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferCreate);

    RENDER_SUBMIT(state, [resID = m_id, pData, dataSize]()
      {
        RT_IndexBuffer * pIB = RT_IndexBuffer::Create(pData, dataSize);
        if (pIB == nullptr)
        {
          LOG_WARN("RT_IndexBuffer::RT_IndexBuffer(): Failed to create index buffer!");
          return;
        }
        RenderThreadData::Instance()->IBOs.insert(resID, pIB);
      });
  }

  Ref<IndexBuffer> IndexBuffer::Create(uint8_t const * a_pData, uint32_t a_count)
  {
    BSR_ASSERT(a_pData != nullptr);
    return Ref<IndexBuffer>(new IndexBuffer(a_pData, IndexDataType::unsigned_8, a_count));
  }

  Ref<IndexBuffer> IndexBuffer::Create(uint16_t const * a_pData, uint32_t a_count)
  {
    BSR_ASSERT(a_pData != nullptr);
    return Ref<IndexBuffer>(new IndexBuffer(a_pData, IndexDataType::unsigned_16, a_count));
  }

  Ref<IndexBuffer> IndexBuffer::Create(uint32_t const * a_pData, uint32_t a_count)
  {
    BSR_ASSERT(a_pData != nullptr);
    return Ref<IndexBuffer>(new IndexBuffer(a_pData, IndexDataType::unsigned_32, a_count));
  }

  IndexBuffer::~IndexBuffer()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_IndexBuffer ** ppIB =  RenderThreadData::Instance()->IBOs.at(resID);
        if (ppIB == nullptr)
        {
          LOG_WARN("IndexBuffer::~IndexBuffer: RefID '{}' does not exist!", resID);
          return;
        }
        delete *ppIB;
        *ppIB = nullptr;
        RenderThreadData::Instance()->IBOs.erase(resID);
      });
  }

  void IndexBuffer::SetData(void const * a_pData, uint32_t a_size, uint32_t a_offset)
  {
    BSR_ASSERT(a_pData != nullptr);

    uint8_t* data = (uint8_t*)RENDER_ALLOCATE(a_size);
    memcpy(data, a_pData, a_size);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferSetData);

    RENDER_SUBMIT(state, [resID = m_id, offset = a_offset, size = a_size, data]()
      {
        RT_IndexBuffer ** ppIB = RenderThreadData::Instance()->IBOs.at(resID);
        if (ppIB == nullptr)
        {
          LOG_WARN("IndexBuffer::SetData(): RefID '{}' does not exist!", resID);
          return;
        }
        (*ppIB)->SetData(data, size, offset);
      });
  }

  void IndexBuffer::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BufferBind);

    RENDER_SUBMIT(state, [resID = m_id]()
      {
        RT_IndexBuffer ** ppIB = RenderThreadData::Instance()->IBOs.at(resID);
        if (ppIB == nullptr)
        {
          LOG_WARN("IndexBuffer::Bind(): RefID '{}' does not exist!", resID);
          return;
        }

        (*ppIB)->Bind();
      });
  }

  uint32_t IndexBuffer::ElementCount() const
  {
    return m_elementCount;
  }

  IndexDataType IndexBuffer::DataType() const
  {
    return m_dataType;
  }
}