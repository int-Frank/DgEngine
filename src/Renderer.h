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

#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include "Memory.h"

#include "PODArray.h"
#include "RT_RendererAPI.h"
#include "RenderState.h"
#include "Group.h"
#include "RenderCommandQueue.h"
#include "MemBuffer.h"
#include "VertexArray.h"
#include "RenderCommon.h"

#define RENDER_SUBMIT(state, ...) ::DgE::Renderer::Instance()->Submit(state, __VA_ARGS__)
#define RENDER_ALLOCATE(size) ::DgE::Renderer::Instance()->Allocate(size)

namespace DgE
{
  struct GlobalRenderState;

  class Renderer
  {
  public:

    static bool Init();
    static void ShutDown();
    static Renderer * Instance();

    Renderer();
    ~Renderer();

    static void Clear();
    static void Clear(float r, float g, float b, float a = 1.0f);
    static void SetClearColor(float r, float g, float b, float a = 1.0f);
    static void SetSissorBox(int x, int y, int w, int h);
    static void Enable(RenderFeature);
    static void Disable(RenderFeature);
    static void DrawIndexed(Ref<VertexArray> const &, RenderMode, uint32_t instanceCount, uint32_t elementCount = 0);

    // Allocates on the temporary buffer. Do not delete!
    // Will be cleared every frame!
    static GlobalRenderState * GetGlobalRenderState();
    static void SetRenderState(GlobalRenderState *);

    //Everything must happen between these two functions.
    void BeginScene();
    void EndScene();

    //Group commands together
    void BeginNewGroup();
    void EndCurrentGroup();

    template<typename FuncT>
    void Submit(RenderState a_state, FuncT&& func)
    {
      DG_ASSERT(std::is_trivially_destructible<FuncT>::value, "FuncT must be trivially destructible");
      RenderCommandFn renderCmd = [](void* ptr)
      {
        auto pFunc = (FuncT*)ptr;
        (*pFunc)();
        pFunc->~FuncT();
      };
      a_state.Set(RenderState::Attr::Group, uint64_t(m_group.GetCurrentID()));
      auto pStorageBuffer = s_instance->m_commandQueue.AllocateForCommand(a_state, renderCmd, sizeof(func));
      new (pStorageBuffer) FuncT(std::forward<FuncT>(func));
    }

    void SwapBuffers();
    void* Allocate(uint32_t);

    //Render thread
    void ExecuteRenderCommands();

  private:

    bool __Init();

  private:

    static Renderer * s_instance;

  private:

    enum class ReturnCode
    {
      None,
      Ready,
      Fail,
    };

    RenderCommandQueue m_commandQueue;
    Group m_group;
  };

}

#endif