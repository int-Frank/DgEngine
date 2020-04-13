//@group Renderer

#ifndef EN_OPENGLCONTEXT_H
#define EN_OPENGLCONTEXT_H

#include "IGraphicsContext.h"

struct SDL_Window;

namespace Engine
{
  typedef void *SDL_GLContext;

  class OpenGLContext : public IGraphicsContext
  {
  public:

    ~OpenGLContext();
    OpenGLContext();

    Core::ErrorCode Init() override;
    Core::ErrorCode ShutDown() override;
    void SetSDLWindow(SDL_Window *);
    void SwapBuffers() override;

  private:

    SDL_Window *  m_pWindow;
    SDL_GLContext m_context;
  };
}


#endif