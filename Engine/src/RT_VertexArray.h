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

#ifndef RT_VERTEXARRAY_H
#define RT_VERTEXARRAY_H

#include <stdint.h>
#include "DgDynamicArray.h"
#include "RT_RendererAPI.h"
#include "ResourceID.h"

namespace Engine
{
  class RT_VertexArray
  {
  public:

    RT_VertexArray();
    ~RT_VertexArray();

    void Init();
    void Destroy();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(RefID id);
    void SetIndexBuffer(RefID id);

    Dg::DynamicArray<RefID> const & GetVertexBuffers() const;
    RefID GetIndexBuffer() const;

  private:

    RendererID m_rendererID;
    Dg::DynamicArray<RefID> m_vertexBuffers; //TODO is this even needed?
    RefID m_indexBuffer; //TODO is this even needed?
    uint32_t m_vertexAttribIndex;
  };
}

#endif