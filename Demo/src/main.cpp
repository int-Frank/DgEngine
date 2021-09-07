
#include <thread>

#include "Engine.h"

#include "common.h"
#include "RenderDemo.h"
#include "GUIDemo.h"

class Game : public DgE::Application
{
public:
  Game(Opts const & a_opts)
    : Application(a_opts)
  {
    uint32_t threadCount = std::thread::hardware_concurrency();
    if (threadCount < 3)
      threadCount = 1;
    else
      threadCount -= 2;

    InitWorkerPool(threadCount);

    void RunTests();
    RunTests();

    PushSystem(new RenderDemo());
    PushSystem(new GUIDemo());
  }

  ~Game()
  {
    DestroyWorkerPool();
  }

};

DgE::Application* DgE::CreateApplication()
{
  Application::Opts opts;
  return new Game(opts);
}
