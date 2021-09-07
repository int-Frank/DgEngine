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

#ifndef IVERTEXARRAY_H
#define IVERTEXARRAY_H

#include "Memory.h"
#include "Buffer.h"
#include "RenderResource.h"

namespace DgE 
{
  class VertexArray : public RenderResource
  {
    VertexArray();
  public:

    static Ref<VertexArray> Create();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void SetVertexAttributeDivisor(uint32_t attrIndex, uint32_t divisor);

    void AddVertexBuffer(Ref<VertexBuffer> const &);
    void SetIndexBuffer(Ref<IndexBuffer> const &);

    Ref<IndexBuffer> const & GetIndexBuffer();

  private:

    Ref<IndexBuffer> m_indexBuffer;
  };
}

#endif