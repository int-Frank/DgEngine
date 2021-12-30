//@group Renderer

#include "OpenGLContext.h"
#include "Framework.h"
#include "glad/glad.h"
#include "Log.h"
#include "DgError.h"
#include "Options.h"
#include "SDL.h"

namespace DgE
{
  Dg::ErrorCode Framework::InitGraphicsContext()
  {
    SetGraphicsContext(new OpenGLContext());
    return Dg::ErrorCode::None;
  }

  OpenGLContext::~OpenGLContext()
  {

  }

  OpenGLContext::OpenGLContext()
    : m_pWindow(nullptr)
    , m_context(nullptr)
  {

  }

  Dg::ErrorCode OpenGLContext::ShutDown()
  {
    SDL_GL_DeleteContext(m_context);
    m_context = nullptr;
    return Dg::ErrorCode::None;
  }

  Dg::ErrorCode OpenGLContext::Init()
  {
    DG_ASSERT(m_pWindow != nullptr, "Render context does not have a window! Make sure you set the window first!");

    // Create OpenGL context
    m_context = SDL_GL_CreateContext(m_pWindow);
    if (m_context == nullptr)
    {
      LOG_ERROR("SDL_GL_CreateContext() Failed!");
      return Dg::ErrorCode::Failure;
    }
    LOG_TRACE("Opengl loaded");

    if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
    {
      LOG_ERROR("Glad failed to log");
      return Dg::ErrorCode::Failure;
    }

    LOG_TRACE("Vendor:   {}", glGetString(GL_VENDOR));
    LOG_TRACE("Renderer: {}", glGetString(GL_RENDERER));
    LOG_TRACE("Version:  {}", glGetString(GL_VERSION));

    return Dg::ErrorCode::None;
  }

  void OpenGLContext::SetSDLWindow(SDL_Window* a_pWindow)
  {
    m_pWindow = a_pWindow;
  }

  void OpenGLContext::SwapBuffers()
  {
    SDL_GL_SwapWindow(m_pWindow);
  }

  void OpenGLContext::Resize(uint32_t w, uint32_t h)
  {
    glViewport(0, 0, w, h);
  }
}
