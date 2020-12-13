//@group Framework

#include "SDL_mouse.h"

#include "Framework.h"
#include "IMouseController.h"

namespace Engine
{
  class FW_SDLMouseController : public IMouseController
  {
  public:

    Dg::ErrorCode Grab();
    Dg::ErrorCode Release();

    void GetPos(int & x, int & y);
    void MoveToPos(int x, int y);

  private:

  };

  Dg::ErrorCode Framework::InitMouseController()
  {
    SetMouseController(new FW_SDLMouseController());
    return Dg::ErrorCode::None;
  }

  Dg::ErrorCode FW_SDLMouseController::Grab()
  {
    return SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? Dg::ErrorCode::None : Dg::ErrorCode::Failure;
  }

  Dg::ErrorCode FW_SDLMouseController::Release()
  {
    return SDL_SetRelativeMouseMode(SDL_FALSE) == 0 ? Dg::ErrorCode::None : Dg::ErrorCode::Failure;
  }

  void FW_SDLMouseController::GetPos(int & a_x, int & a_y)
  {
    SDL_GetMouseState(&a_x, &a_y);
  }

  void FW_SDLMouseController::MoveToPos(int a_x, int a_y)
  {
    SDL_WarpMouseInWindow(nullptr, a_x, a_y);
  }
}
