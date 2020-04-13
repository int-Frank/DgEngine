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

#include <glad/glad.h>
#include "RT_VertexArray.h"
#include "Buffer.h"
#include "RT_Buffer.h"
#include "RenderThreadData.h"
#include "ShaderUtils.h"

namespace Engine
{
  RT_VertexArray::RT_VertexArray()
    : m_rendererID(0)
    , m_indexBuffer(INVALID_REFID)
    , m_vertexAttribIndex(0)
  {

  }

  RT_VertexArray::~RT_VertexArray()
  {

  }

  void RT_VertexArray::Init()
  {
    BSR_ASSERT(m_rendererID == 0, "Already initialized!");
    glCreateVertexArrays(1, &m_rendererID);
  }

  void RT_VertexArray::Destroy()
  {
    BSR_ASSERT(m_rendererID != 0, "Trying to destroy a verex array that wasn't initialized!");
    glDeleteVertexArrays(1, &m_rendererID);
    m_rendererID = 0;
  }

  void RT_VertexArray::Bind() const
  {
    glBindVertexArray(m_rendererID);
  }

  void RT_VertexArray::Unbind() const
  {
    glBindVertexArray(0);
  }

  void RT_VertexArray::AddVertexBuffer(RefID a_id)
  {
    RT_VertexBuffer* pVB = RenderThreadData::Instance()->VBOs.at(a_id);
    if (pVB == nullptr)
    {
      LOG_WARN("RT_VertexArray::AddVertexBuffer(): Failed to find the index buffer! RefID : {}", a_id);
      return;
    }

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

  void RT_VertexArray::SetIndexBuffer(RefID a_id)
  {
    RT_IndexBuffer * pIB = RenderThreadData::Instance()->IBOs.at(a_id);
    if (pIB == nullptr)
    {
      LOG_WARN("RT_VertexArray::SetIndexBuffer(): Failed to find the index buffer! RefID : {}", a_id);
      return;
    }

    Bind();
    pIB->Bind();
    m_indexBuffer = a_id;
  }

  Dg::DynamicArray<RefID> const & RT_VertexArray::GetVertexBuffers() const
  {
    return m_vertexBuffers;
  }

  RefID RT_VertexArray::GetIndexBuffer() const
  {
    return m_indexBuffer;
  }
}