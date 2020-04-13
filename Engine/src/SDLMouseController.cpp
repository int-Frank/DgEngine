//@group Framework

#include "SDL_mouse.h"

#include "Framework.h"
#include "IMouseController.h"

namespace Engine
{
  class FW_SDLMouseController : public IMouseController
  {
  public:

    Core::ErrorCode Grab();
    Core::ErrorCode Release();

    void GetPos(int & x, int & y);
    void MoveToPos(int x, int y);

  private:

  };

  void Framework::InitMouseController()
  {
    SetMouseController(new FW_SDLMouseController());
  }

  Core::ErrorCode FW_SDLMouseController::Grab()
  {
    return SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? Core::EC_None : Core::EC_Error;
  }

  Core::ErrorCode FW_SDLMouseController::Release()
  {
    return SDL_SetRelativeMouseMode(SDL_FALSE) == 0 ? Core::EC_None : Core::EC_Error;
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
