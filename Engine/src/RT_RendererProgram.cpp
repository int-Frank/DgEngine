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

//#include <fstream>

#include "RT_RendererProgram.h"
#include "RT_Texture.h"
#include "RenderThreadData.h"
#include "core_ErrorCodes.h"
#include "core_Log.h"
#include "core_utils.h"
#include "core_Assert.h"
#include "DgStringFunctions.h"
#include "Serialize.h"
#include "ResourceManager.h"

#include "glad/glad.h"

//TODO Parse uniform blocks, shader storage blocks

namespace Engine
{
  //--------------------------------------------------------------------------------------------------
  // RT_RendererProgram
  //--------------------------------------------------------------------------------------------------

  //RT_RendererProgram::RT_RendererProgram()
  //  : m_rendererID(0)
  //  , m_loaded(false)
  //{
  //
  //}

  RT_RendererProgram::RT_RendererProgram(ResourceID a_shaderDataID)
    : m_rendererID(0)
  {
    m_pShaderData = ResourceManager::Instance()->GetResource<ShaderData>(a_shaderDataID);
    if (m_pShaderData == nullptr)
    {
      LOG_WARN("RT_RendererProgram failed to find shader data resource!");
      throw;
    }

    if (!CompileAndUploadShader())
      throw;

    ResolveUniforms();
  }

  RT_RendererProgram::~RT_RendererProgram()
  {
    glDeleteProgram(m_rendererID);
    m_rendererID = 0;
    m_uniformLocations.clear();
  }

  void RT_RendererProgram::Bind() const
  {
    glUseProgram(m_rendererID);
  }

  void RT_RendererProgram::Unbind() const
  {
    glUseProgram(0);
  }

  RT_RendererProgram * RT_RendererProgram::Create(ResourceID a_shaderDataID)
  {
    RT_RendererProgram * pResult = nullptr;

    try
    {
      pResult = new RT_RendererProgram(a_shaderDataID);
    }
    catch (...)
    {
      pResult = nullptr;
    }

    return pResult;
  }

  bool RT_RendererProgram::CompileAndUploadShader()
  {
    if (m_pShaderData == nullptr)
      return false;

    bool result = true;
    Dg::DynamicArray<GLuint> shaderRendererIDs;

    GLuint program = glCreateProgram();
    for (int i = 0; i < ShaderDomain_COUNT; i++)
    {
      GLenum type = ShaderDomainToOpenGLType(ShaderDomain(i));
      ShaderDomain domain = static_cast<ShaderDomain>(i);
      std::string const & source = m_pShaderData->GetShaderSource().Get(domain);

      if (source.empty())
        continue;

      GLuint shaderRendererID = glCreateShader(type);
      GLchar const * sourceCstr = (GLchar const *)source.c_str();
      glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

      glCompileShader(shaderRendererID);

      GLint isCompiled = 0;
      glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
      if (isCompiled == GL_FALSE)
      {
        GLint maxLength = 0;
        glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

        LOG_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(shaderRendererID);

        BSR_ASSERT(false, "Failed");
        result = false;
      }

      shaderRendererIDs.push_back(shaderRendererID);
      glAttachShader(program, shaderRendererID);
    }

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
      LOG_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

      // We don't need the program anymore.
      glDeleteProgram(program);
      // Don't leak shaders either.
      for (auto id : shaderRendererIDs)
        glDeleteShader(id);
      result = false;
    }

    // Always detach shaders after a successful link.
    for (auto id : shaderRendererIDs)
      glDetachShader(program, id);

    m_rendererID = program;

    LOG_DEBUG("Successfully created program");

    return result;
  }

  void RT_RendererProgram::ResolveUniforms()
  {
    if (m_pShaderData == nullptr)
      return;

    glUseProgram(m_rendererID);

    uint32_t sampler = 0;
    Index ind = 0;
    ShaderUniformList const & rUniforms(m_pShaderData->GetUniforms());
    for (ShaderUniformList::const_iterator it = rUniforms.cbegin(); it != rUniforms.cend(); it++)
    {
      if (it->GetType() == ShaderDataType::TEXTURE2D)
      {
        int32_t location = GetUniformLocation(it->GetName());
        m_textureBindingPoints.insert(ind, sampler);

        if (it->GetCount() == 1)
        {
          if (location != -1)
            UploadUniformSingle(location, ShaderDataType::INT, &sampler);

          sampler++;
        }
        else if (it->GetCount() > 1)
        {
          uint32_t count = it->GetCount();
          int * samplers = new int[count];
          for (uint32_t s = 0; s < count; s++, sampler++)
            samplers[s] = sampler;
          UploadUniformArray(location, ShaderDataType::INT, samplers, count);
          delete[] samplers;
        }
      }
      else
      {
        m_uniformLocations.push_back(GetUniformLocation(it->GetName()));
      }
      ind++;
    }
  }

  int32_t RT_RendererProgram::GetUniformLocation(std::string const& a_name) const
  {
    int32_t result = glGetUniformLocation(m_rendererID, a_name.c_str());
    if (result == -1)
      LOG_WARN("Could not find uniform '{0}' in shader", a_name.c_str());
    return result;
  }

  void RT_RendererProgram::UploadTexture(TextureUnit a_textureUnit, RenderResourceID const * a_textureIDs, uint32_t a_count)
  {
    uint32_t textureUnit = a_textureUnit;
    for (uint32_t i = 0; i < a_count; i++)
    {
      RenderResourceID id = a_textureIDs[i];

      RT_Texture2D **ppTexture = RenderThreadData::Instance()->textures.at(id);
      
      if (*ppTexture != nullptr)
        (*ppTexture)->Bind(textureUnit);
      textureUnit++;
    }
  }

  void RT_RendererProgram::UploadUniformBuffer(byte const* a_pbuf)
  {
    if (m_pShaderData == nullptr || a_pbuf == nullptr)
      return;

    Bind();

    for (uint32_t i = 0; i < (uint32_t)m_pShaderData->GetUniforms().size(); i++)
    {
      ShaderUniformDeclaration const * pdecl = &m_pShaderData->GetUniforms()[i];
      uint32_t offset = pdecl->GetDataOffset();
      UniformBufferElementHeader header;
      void const * buf = header.Deserialize(a_pbuf);
      if (header.GetSize() == 0)
        continue;

      uint32_t count = header.GetSize() / SizeOfShaderDataType(pdecl->GetType());
      if (pdecl->GetType() == ShaderDataType::TEXTURE2D)
      {
        TextureUnit const * pUnit = m_textureBindingPoints.at(i);
        if (pUnit != nullptr)
          UploadTexture(*pUnit, (RenderResourceID *)buf, count);
      }
      else
      {
        UploadUniform(i, buf, count);
      }
    }
  }

  void RT_RendererProgram::UploadUniformSingle(int a_location, ShaderDataType a_type,  void const* a_pbuf)
  {
    // TODO FINISH THESE!
    switch (a_type)
    {
      case ShaderDataType::BOOL:
      case ShaderDataType::INT:
      {
        glUniform1i(a_location, *static_cast<int const*>(a_pbuf));
        break;
      }
      case ShaderDataType::FLOAT:
      {
        glUniform1f(a_location, *static_cast<float const *>(a_pbuf));
        break;
      }
      case ShaderDataType::VEC2:
      {
        glUniform2fv(a_location, 1, static_cast<float const *>(a_pbuf));
        break;
      }
      case ShaderDataType::VEC4:
      {
        glUniform4fv(a_location, 1, static_cast<float const *>(a_pbuf));
        break;
      }
    }
  }

  void RT_RendererProgram::UploadUniformArray(int a_location, ShaderDataType a_type,
                                              void const* a_pbuf, uint32_t a_count)
  {
    switch (a_type)
    {
      case ShaderDataType::BOOL:
      case ShaderDataType::INT:
      {
        glUniform1iv(a_location, a_count, static_cast<int const*>(a_pbuf));
        break;
      }
      case ShaderDataType::FLOAT:
      {
        glUniform1fv(a_location, a_count, static_cast<float const*>(a_pbuf));
        break;
      }
    }
  }

  void RT_RendererProgram::UploadUniform(uint32_t a_index, void const * a_pbuf, uint32_t a_count)
  {
    ShaderUniformDeclaration const * pdecl = &m_pShaderData->GetUniforms()[a_index];
    BSR_ASSERT(a_count <= pdecl->GetCount());

    if (pdecl->IsArray())
      UploadUniformArray(m_uniformLocations[a_index], pdecl->GetType(), a_pbuf, a_count);
    else
      UploadUniformSingle(m_uniformLocations[a_index], pdecl->GetType(), a_pbuf);
  }

  void RT_RendererProgram::UploadUniform(std::string const& a_name, void const* a_pbuf, uint32_t a_size)
  {
    if (m_pShaderData == nullptr || a_pbuf == nullptr)
      return;

    uint32_t index = m_pShaderData->FindUniformIndex(a_name);
    if (index == INVALID_INDEX)
    {
      LOG_WARN("Failed to find Uniform '{}'", a_name.c_str());
      return;
    }

    ShaderUniformDeclaration const * pdecl = &m_pShaderData->GetUniforms()[index];
    uint32_t elementSize = SizeOfShaderDataType(pdecl->GetType());
    uint32_t count = a_size / elementSize;

    //TODO should this be here, or do we leave it up to the user to bind before uploading uniforms?
    Bind();

    UploadUniform(index, a_pbuf, count);
  }
}
