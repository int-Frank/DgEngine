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

#ifndef RENDERCOMMANDQUEUE_H
#define RENDERCOMMANDQUEUE_H

#include "Memory.h"
#include <stdint.h>

#include "DgDynamicArray.h"
#include "PODArray.h"
#include "RenderState.h"
#include "MemBuffer.h"

namespace Engine
{
  class RenderSortCriterion
  {

  };

  typedef void(*RenderCommandFn)(void*);

  class RenderCommandQueue
  {
    static size_t const s_cmdBufSize = 1 * 1024 * 1024;
    static size_t const s_outBufSize = 1 * 1024 * 1024;
    static size_t const s_memBufSize = 10 * 1024 * 1024;

  public:

    RenderCommandQueue();
    ~RenderCommandQueue();

    //Main thread...
    void * AllocateForCommand(RenderState, RenderCommandFn, uint32_t size);
    void* Allocate(uint32_t size);
    void PushCriterion(Ref<RenderSortCriterion>);
    void ClearCriterion();

    //Render thread
    void Swap();
    void Sort();
    void Execute();

  private:

    struct SubArray
    {
      uint32_t index;
      uint32_t count;
    };

    struct Buffer
    {
      Buffer(size_t memBufSize);
      void Clear();

      MemBuffer       buf;
      PODArray<void*> allocs;
    };

    PODArray<uint32_t>  m_sortedCommands;
    int                 m_writeIndex;
    Buffer              m_commandBuffer[2];
    MemBuffer           m_mem[2];

    Dg::DynamicArray<Ref<RenderSortCriterion>> m_sortCriterion;
    PODArray<SubArray>                         m_sortableSegs;

  };
}

#endif