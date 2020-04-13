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
#include "RT_RendererAPI.h"
#include "core_Log.h"
#include "core_Assert.h"

namespace Engine
{
  static void OpenGLLogMessage(GLenum a_source, GLenum a_type, GLuint a_id, GLenum a_severity, 
                               GLsizei a_length, const GLchar* a_message, const void* a_userParam)
  {
    if (a_severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    {
      LOG_ERROR("{0}", a_message);
      BSR_ASSERT(false, "");
    }
    else
    {
      // LOG_TRACE("{0}", message);
    }
  }

  void RendererAPI::Init()
  {
    glDebugMessageCallback(OpenGLLogMessage, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& caps = RendererAPI::GetCapabilities();

    caps.vendor = (const char*)glGetString(GL_VENDOR);
    caps.renderer = (const char*)glGetString(GL_RENDERER);
    caps.version = (const char*)glGetString(GL_VERSION);

    glGetIntegerv(GL_MAX_SAMPLES, &caps.maxSamples);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.maxAnisotropy);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &caps.maxTextureUnits);

    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &caps.maxVertexUniformBlocks);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &caps.maxGeometryUniformBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &caps.maxFragmentUniformBlocks);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &caps.maxUniformBlockSize);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &caps.maxUniformBufferBindings);

    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &caps.maxVertexShaderStorageBlocks);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &caps.maxGeometryShaderStorageBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &caps.maxFragmentShaderStorageBlocks);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &caps.maxShaderStorageBlockSize);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &caps.maxShaderStorageBufferBindings);
    
    GLenum error = glGetError();
    while (error != GL_NO_ERROR)
    {
      LOG_ERROR("OpenGL Error {0}", error);
      error = glGetError();
    }

    LoadRequiredAssets();
  }

  void RendererAPI::ShutDown()
  {
  }

  void RendererAPI::LoadRequiredAssets()
  {
  }

  void RendererAPI::Clear()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void RendererAPI::Clear(float r, float g, float b, float a)
  {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void RendererAPI::SetClearColor(float r, float g, float b, float a)
  {
    glClearColor(r, g, b, a);
  }

  void RendererAPI::DrawIndexed(unsigned int count, bool depthTest)
  {
    if (!depthTest)
      glDisable(GL_DEPTH_TEST);

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

    if (!depthTest)
      glEnable(GL_DEPTH_TEST);
  }
}