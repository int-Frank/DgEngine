//@group Framework


#include "SDL.h"

#include "Framework.h"
#include "Options.h"
#include "Log.h"
#include "IWindow.h"
#include "Options.h"
#include "OpenGLContext.h"
#include "glad/glad.h"

#define OPENGL_MAJOR 4
#define OPENGL_MINOR 6

namespace DgE
{
  class FW_SDLWindow : public IWindow
  {
  public:

    FW_SDLWindow();
    ~FW_SDLWindow();

    void Update();

    void SwapBuffers() override;
    void SetVSync(bool) override;
    bool IsVSync() const override;

    bool IsInit() const override;
    Dg::ErrorCode Init(WindowProps const & props = WindowProps());
    void Destroy() override;

    void GetDimensions(int & w, int & h) override;

  private:

    SDL_Window * m_pWindow;
  };

  Dg::ErrorCode Framework::InitWindow()
  {
    SetWindow(new FW_SDLWindow());
    return Dg::ErrorCode::None;
  }

  FW_SDLWindow::FW_SDLWindow()
    : m_pWindow(nullptr)
  {

  }

  FW_SDLWindow::~FW_SDLWindow()
  {
    Destroy();
  }

  void FW_SDLWindow::Update()
  {
    Framework::Instance()->GetGraphicsContext()->SwapBuffers();
  }

  void FW_SDLWindow::SwapBuffers()
  {
    SDL_GL_SwapWindow(m_pWindow);
  }

  void FW_SDLWindow::SetVSync(bool a_val)
  {
    if (a_val)
      SDL_GL_SetSwapInterval(1);
    else
      SDL_GL_SetSwapInterval(0);
  }

  bool FW_SDLWindow::IsVSync() const
  {
    return SDL_GL_GetSwapInterval() == 1;
  }

  bool FW_SDLWindow::IsInit() const
  {
    return m_pWindow != nullptr;
  }

  Dg::ErrorCode FW_SDLWindow::Init(WindowProps const & a_props)
  {
    DG_ASSERT(m_pWindow == nullptr, "FW_SDLWindow already initialised!");

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    m_pWindow = SDL_CreateWindow(a_props.name.c_str(), 100, 100,
      a_props.width, a_props.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(m_pWindow == nullptr)
    {
      LOG_ERROR("Failed to create window!");
      return Dg::ErrorCode::Failure;
    }

    OpenGLContext * pContext = dynamic_cast<OpenGLContext*>
      (
        &*Framework::Instance()->GetGraphicsContext()
      );

    pContext->SetSDLWindow(m_pWindow);

    return Dg::ErrorCode::None;
  }

  void FW_SDLWindow::Destroy()
  {
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;
  }

  void FW_SDLWindow::GetDimensions(int & a_w, int & a_h)
  {
    SDL_GetWindowSize(m_pWindow, &a_w, &a_h);
  }
}
