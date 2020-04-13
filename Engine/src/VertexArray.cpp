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

#include "glad/glad.h"

#include "Renderer.h"
#include "RT_RendererAPI.h"
#include "VertexArray.h"
#include "core_Assert.h"
#include "Memory.h"
#include "Resource.h"
#include "Message.h"
#include "MessageBus.h"
#include "RenderThreadData.h"

namespace Engine
{
  

  VertexArray::VertexArray()
  {

  }

  void VertexArray::Init()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayCreate);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]() mutable
      {
        ::Engine::RT_VertexArray va;
        va.Init();
        ::Engine::RenderThreadData::Instance()->VAOs.insert(resID, va);
      });
  }

  Ref<VertexArray> VertexArray::Create()
  {
    VertexArray * p = new VertexArray();
    Ref<VertexArray> ref(p);
    p->Init();
    return ref;
  }

  VertexArray::~VertexArray()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayDelete);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]() mutable
      {
        RT_VertexArray * pVA =  RenderThreadData::Instance()->VAOs.at(resID);
        if (pVA == nullptr)
        {
          LOG_WARN("VertexArray::~VertexArray: RefID '{}' does not exist!", resID);
          return;
        }
        pVA->Destroy();
        ::Engine::RenderThreadData::Instance()->VAOs.erase(resID);
      });
  }

  void VertexArray::Bind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayBind);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]() mutable
      {
        RT_VertexArray * pID =  RenderThreadData::Instance()->VAOs.at(resID);
        if (pID == nullptr)
        {
          LOG_WARN("VertexArray::Bind(): RefID '{}' does not exist!", resID);
          return;
        }
        pID->Bind();
      });
  }

  void VertexArray::Unbind() const
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayUnbind);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]()
      {
        RT_VertexArray* pID =  RenderThreadData::Instance()->VAOs.at(resID);
        if (pID == nullptr)
        {
          LOG_WARN("VertexArray::Unbind(): RefID '{}' does not exist!", resID);
          return;
        }
        pID->Unbind();
      });
  }

  void VertexArray::AddVertexBuffer(Ref<VertexBuffer> const & a_vertexBuffer)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArrayAddVertexBuffer);

    RENDER_SUBMIT(state, [vbID = a_vertexBuffer->GetRefID().GetID(), vaoID = GetRefID().GetID()]() mutable
      {
        RT_VertexArray* pID =  RenderThreadData::Instance()->VAOs.at(vaoID);
        if (pID == nullptr)
        {
          LOG_WARN("VertexArray::AddVertexBuffer(): RefID '{}' does not exist!", vaoID);
          return;
        }
        pID->AddVertexBuffer(vbID);
      });
  }

  void VertexArray::SetIndexBuffer(Ref<IndexBuffer> const & a_indexBuffer)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::VertexArraySetIndexBuffer);

    RENDER_SUBMIT(state, [iboID = a_indexBuffer->GetRefID().GetID(), vaoID = GetRefID().GetID()]()
    {
      RT_VertexArray* pID =  RenderThreadData::Instance()->VAOs.at(vaoID);
      if (pID == nullptr)
      {
        LOG_WARN("VertexArray::SetIndexBuffer(): RefID '{}' does not exist!", vaoID);
        return;
      }
      pID->SetIndexBuffer(iboID);
    });
  }
}