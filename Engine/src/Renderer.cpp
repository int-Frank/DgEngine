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
#include "Log.h"
#include "Options.h"
#include "Framework.h"
#include "Log.h"
#include "RT_RendererAPI.h"
#include "RenderThread.h"
#include "Memory.h"

namespace DgE
{
  struct GlobalRenderState
  {
    bool RenderFeatureState[(uint32_t)RenderFeature::COUNT];
    int sissorBox[4];
  };

  GlobalRenderState g_renderState ={};

  Renderer* Renderer::s_instance = nullptr;

  Renderer * Renderer::Instance()
  {
    DG_ASSERT(s_instance != nullptr, "Renderer not initialised!");
    return s_instance;
  }

  bool Renderer::__Init()
  {
    return true;
  }

  bool Renderer::Init()
  {
    DG_ASSERT(s_instance == nullptr, "Renderer instance already initialised!");
    s_instance = new Renderer();
    return s_instance->__Init();
  }

  void Renderer::ShutDown()
  {
    delete s_instance;
    s_instance = nullptr;
  }

  Renderer::Renderer()
  {
    
  }

  Renderer::~Renderer()
  {

  }

  void Renderer::DrawIndexed(Ref<VertexArray> const & a_va, RenderMode a_mode, uint32_t a_instanceCount, uint32_t a_elementCount)
  {
    DG_ASSERT(a_va.get() != nullptr);
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::Draw);

    uint32_t count = a_elementCount == 0 ? a_va->GetIndexBuffer()->ElementCount() : a_elementCount;

    RENDER_SUBMIT(state, [a_mode, dataType = a_va->GetIndexBuffer()->DataType(), a_instanceCount, count]()
      {
        RendererAPI::DrawIndexed(a_mode, dataType, a_instanceCount, count);
      });
  }

  void Renderer::Clear()
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::Clear);

    RENDER_SUBMIT(state, []()
      {
        RendererAPI::Clear();
      });
  }

  void Renderer::Clear(float a_r, float a_g, float a_b, float a_a)
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::Clear);

    RENDER_SUBMIT(state, [r = a_r, g = a_g, b = a_b, a = a_a]()
      {
        RendererAPI::Clear(r, g, b, a);
      });
  }

  void Renderer::SetClearColor(float a_r, float a_g, float a_b, float a_a)
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::SetClearColor);

    RENDER_SUBMIT(state, [r = a_r, g = a_g, b = a_b, a = a_a]()
      {
        RendererAPI::SetClearColor(r, g, b, a);
      });
  }

  void Renderer::SetSissorBox(int a_x, int a_y, int a_w, int a_h)
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::SetClearColor);

    RENDER_SUBMIT(state, [x = a_x, y = a_y, w = a_w, h = a_h]()
      {
        RendererAPI::SetSissorBox(x, y, w, h);
      });

    g_renderState.sissorBox[0] = a_x;
    g_renderState.sissorBox[1] = a_y;
    g_renderState.sissorBox[2] = a_w;
    g_renderState.sissorBox[3] = a_h;
  }

  void Renderer::Enable(RenderFeature a_feature)
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::EnableFeature);

    RENDER_SUBMIT(state, [a_feature]() { RendererAPI::Enable(a_feature); });
    g_renderState.RenderFeatureState[static_cast<uint32_t>(a_feature)] = true;
  }

  void Renderer::Disable(RenderFeature a_feature)
  {
    DgE::RenderState state = DgE::RenderState::Create();
    state.Set<DgE::RenderState::Attr::Type>(DgE::RenderState::Type::Command);
    state.Set<DgE::RenderState::Attr::Command>(DgE::RenderState::Command::DisableFeature);

    RENDER_SUBMIT(state, [a_feature]() { RendererAPI::Disable(a_feature); });
    g_renderState.RenderFeatureState[static_cast<uint32_t>(a_feature)] = false;
  }

  void Renderer::BeginScene()
  {
    m_group.Reset();
  }

  void Renderer::EndScene()
  {

  }

  void * Renderer::Allocate(uint32_t a_size)
  {
    return m_commandQueue.Allocate(a_size);
  }

  void Renderer::SwapBuffers()
  {
    m_commandQueue.Swap();
  }

  void Renderer::ExecuteRenderCommands()
  {
    m_commandQueue.Execute();
  }

  void Renderer::BeginNewGroup()
  {
    m_group.BeginNewGroup();
  }

  void Renderer::EndCurrentGroup()
  {
    m_group.EndCurrentGroup();
  }

  GlobalRenderState * Renderer::GetGlobalRenderState()
  {
    GlobalRenderState * pState = (GlobalRenderState *)TBUFAlloc(sizeof(GlobalRenderState));
    new (pState) GlobalRenderState(g_renderState);
    return pState;
  }

  void Renderer::SetRenderState(GlobalRenderState * a_state)
  {
    if (a_state == nullptr)
      return;

    for (uint32_t i = 0; i < static_cast<uint32_t>(RenderFeature::COUNT); i++)
    {
      if (g_renderState.RenderFeatureState[i] != a_state->RenderFeatureState[i])
      {
        if (a_state->RenderFeatureState[i])
          Enable(static_cast<RenderFeature>(i));
        else
          Disable(static_cast<RenderFeature>(i));
      }
    }

    for (uint32_t i = 0; i < 4; i++)
      g_renderState.sissorBox[i] = a_state->sissorBox[i];
  }
}