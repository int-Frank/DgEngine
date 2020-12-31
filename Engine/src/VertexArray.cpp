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


#include "Renderer.h"
#include "RT_RendererAPI.h"
#include "VertexArray.h"
#include "BSR_Assert.h"
#include "Memory.h"
#include "Message.h"
#include "MessageBus.h"
#include "RenderThreadData.h"

#include "glad/glad.h"

namespace Engine
{
  VertexArray::VertexArray()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayCreate);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        ::Engine::RT_VertexArray * pVA = ::Engine::RT_VertexArray::Create();
        ::Engine::RenderThreadData::Instance()->VAOs.insert(resID, pVA);
      });
  }

  Ref<VertexArray> VertexArray::Create()
  {
    return Ref<VertexArray>(new VertexArray());
  }

  VertexArray::~VertexArray()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayDelete);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_VertexArray ** ppVA =  RenderThreadData::Instance()->VAOs.at(resID);
        if (ppVA == nullptr)
        {
          LOG_WARN("VertexArray::~VertexArray: RefID '{}' does not exist!", resID);
          return;
        }
        delete (*ppVA);
        *ppVA = nullptr;
        ::Engine::RenderThreadData::Instance()->VAOs.erase(resID);
      });
  }

  void VertexArray::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayBind);

    RENDER_SUBMIT(state, [resID = m_id]() mutable
      {
        RT_VertexArray ** ppID =  RenderThreadData::Instance()->VAOs.at(resID);
        if (ppID == nullptr)
        {
          LOG_WARN("VertexArray::Bind(): RefID '{}' does not exist!", resID);
          return;
        }
        (*ppID)->Bind();
      });
  }

  void VertexArray::Unbind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayUnbind);

    RENDER_SUBMIT(state, [resID = m_id]()
      {
        RT_VertexArray ** ppID =  RenderThreadData::Instance()->VAOs.at(resID);
        if (ppID == nullptr)
        {
          LOG_WARN("VertexArray::Unbind(): RefID '{}' does not exist!", resID);
          return;
        }
        (*ppID)->Unbind();
      });
  }

  void VertexArray::SetVertexAttributeDivisor(uint32_t a_attrIndex, uint32_t a_divisor)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArray);

    RENDER_SUBMIT(state, [vaoID = m_id, a_attrIndex, a_divisor]()
    {
      RT_VertexArray ** ppVA =  RenderThreadData::Instance()->VAOs.at(vaoID);
      if (ppVA == nullptr)
      {
        LOG_WARN("VertexArray::SetVertexAttributeDivisor(): RefID '{}' does not exist!", vaoID);
        return;
      }
      (*ppVA)->SetVertexAttributeDivisor(a_attrIndex, a_divisor);
    });
  }

  void VertexArray::AddVertexBuffer(Ref<VertexBuffer> const & a_vertexBuffer)
  {
    BSR_ASSERT(a_vertexBuffer.get() != nullptr);
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayAddVertexBuffer);

    RENDER_SUBMIT(state, [vbID = a_vertexBuffer->GetID(), vaoID = m_id]() mutable
      {
        RT_VertexArray ** ppID =  RenderThreadData::Instance()->VAOs.at(vaoID);
        if (ppID == nullptr)
        {
          LOG_WARN("VertexArray::AddVertexBuffer(): RefID '{}' does not exist!", vaoID);
          return;
        }
        (*ppID)->AddVertexBuffer(vbID);
      });
  }

  void VertexArray::SetIndexBuffer(Ref<IndexBuffer> const & a_indexBuffer)
  {
    BSR_ASSERT(a_indexBuffer.get() != nullptr);
    m_indexBuffer = a_indexBuffer;

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArraySetIndexBuffer);

    RENDER_SUBMIT(state, [iboID = a_indexBuffer->GetID(), vaoID = m_id]()
    {
      RT_VertexArray ** ppID =  RenderThreadData::Instance()->VAOs.at(vaoID);
      if (ppID == nullptr)
      {
        LOG_WARN("VertexArray::SetIndexBuffer(): RefID '{}' does not exist!", vaoID);
        return;
      }
      (*ppID)->SetIndexBuffer(iboID);
    });
  }

  Ref<IndexBuffer> const & VertexArray::GetIndexBuffer()
  {
    return m_indexBuffer;
  }
}