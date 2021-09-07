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
#include "RenderResource.h"

namespace DgE
{
  class RT_VertexArray
  {
    RT_VertexArray();
  public:

    ~RT_VertexArray();

    static RT_VertexArray * Create();

    void Bind() const;
    void Unbind() const;

    void SetVertexAttributeDivisor(uint32_t attrIndex, uint32_t divisor);

    void AddVertexBuffer(RenderResourceID id);
    void SetIndexBuffer(RenderResourceID id);

    Dg::DynamicArray<RenderResourceID> const & GetVertexBuffers() const;
    RenderResourceID GetIndexBuffer() const;

  private:

    RendererID m_rendererID;
    Dg::DynamicArray<RenderResourceID> m_vertexBuffers; //TODO is this even needed?
    RenderResourceID m_indexBuffer; //TODO is this even needed?
    uint32_t m_vertexAttribIndex;
  };
}

#endif