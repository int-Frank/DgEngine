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

#include "RT_VertexArray.h"
#include "Buffer.h"
#include "RT_Buffer.h"
#include "RenderThreadData.h"
#include "ShaderUtils.h"
#include <glad/glad.h>

namespace Engine
{
  RT_VertexArray::RT_VertexArray()
    : m_rendererID(0)
    , m_indexBuffer(INVALID_RENDER_RESOURE_ID)
    , m_vertexAttribIndex(0)
  {
    glCreateVertexArrays(1, &m_rendererID);
  }

  RT_VertexArray::~RT_VertexArray()
  {
    glDeleteVertexArrays(1, &m_rendererID);
  }

  RT_VertexArray * RT_VertexArray::Create()
  {
    return new RT_VertexArray();
  }

  void RT_VertexArray::Bind() const
  {
    glBindVertexArray(m_rendererID);
  }

  void RT_VertexArray::Unbind() const
  {
    glBindVertexArray(0);
  }

  void RT_VertexArray::SetVertexAttributeDivisor(uint32_t a_attrIndex, uint32_t a_divisor)
  {
    Bind();
    // TODO log OpenGL errors
    glVertexAttribDivisor(a_attrIndex, a_divisor);
  }

  void RT_VertexArray::AddVertexBuffer(RenderResourceID a_id)
  {
    RT_VertexBuffer** ppVB = RenderThreadData::Instance()->VBOs.at(a_id);
    if (ppVB == nullptr)
    {
      LOG_WARN("RT_VertexArray::AddVertexBuffer(): Failed to find the index buffer! RefID : {}", a_id);
      return;
    }
    RT_VertexBuffer *pVB = *ppVB;
    BSR_ASSERT(pVB != nullptr, "A vertex Buffer in the render data is null!");
    BSR_ASSERT(pVB->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
    Bind();
    pVB->Bind();

    for (auto it = pVB->GetLayout().begin(); it != pVB->GetLayout().end(); it++)
    {
      ShaderDataBaseType baseType = GetShaderDataBaseType(it->type);
      GLenum glBaseType = ShaderDataBaseTypeToOpenGLType(baseType);
      glEnableVertexAttribArray(m_vertexAttribIndex);
      if (glBaseType == GL_INT)
      {
        glVertexAttribIPointer(m_vertexAttribIndex,
          it->GetComponentCount(),
          glBaseType,
          pVB->GetLayout().GetStride(),
          (const void*)(intptr_t)it->offset);
      }
      else
      {
        // TODO matrices need to be set row by row
        glVertexAttribPointer(m_vertexAttribIndex,
          it->GetComponentCount(),
          glBaseType,
          it->normalized ? GL_TRUE : GL_FALSE,
          pVB->GetLayout().GetStride(),
          (const void*)(intptr_t)it->offset);
      }
      m_vertexAttribIndex++;
    }
    m_vertexBuffers.push_back(a_id);
  }

  void RT_VertexArray::SetIndexBuffer(RenderResourceID a_id)
  {
    RT_IndexBuffer ** ppIB = RenderThreadData::Instance()->IBOs.at(a_id);
    if (ppIB == nullptr)
    {
      LOG_WARN("RT_VertexArray::SetIndexBuffer(): Failed to find the index buffer! RefID : {}", a_id);
      return;
    }

    Bind();
    (*ppIB)->Bind();
    m_indexBuffer = a_id;
  }

  Dg::DynamicArray<RenderResourceID> const & RT_VertexArray::GetVertexBuffers() const
  {
    return m_vertexBuffers;
  }

  RenderResourceID RT_VertexArray::GetIndexBuffer() const
  {
    return m_indexBuffer;
  }
}