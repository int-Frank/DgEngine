//@group Renderer

#ifndef EN_OPENGLCONTEXT_H
#define EN_OPENGLCONTEXT_H

#include <stdint.h>
#include "IGraphicsContext.h"

struct SDL_Window;

namespace DgE
{
  typedef void *SDL_GLContext;

  class OpenGLContext : public IGraphicsContext
  {
  public:

    ~OpenGLContext();
    OpenGLContext();

    Dg::ErrorCode Init() override;
    Dg::ErrorCode ShutDown() override;
    void SetSDLWindow(SDL_Window *);
    void SwapBuffers() override;
    void Resize(uint32_t w, uint32_t h) override;

  private:

    SDL_Window *  m_pWindow;
    SDL_GLContext m_context;
  };
}


#endif