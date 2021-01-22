
#include "Engine.h"

#include "RenderDemo.h"
#include "GUIDemo.h"

class Game : public Engine::Application
{
public:
  Game(Opts const & a_opts)
    : Application(a_opts)
  {
    void RunTests();
    RunTests();

    PushSystem(new RenderDemo());
    PushSystem(new GUIDemo());
  }

  ~Game()
  {

  }

};

Engine::Application* Engine::CreateApplication()
{
  Application::Opts opts;
  return new Game(opts);
}
