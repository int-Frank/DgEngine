//@group Core

#ifndef EN_APPLICATION_H
#define EN_APPLICATION_H

#include <string>
#include "Layer.h"

namespace Engine
{
  class Layer;
  class IWindow;

  class Application
  {
  public:

    enum
    {
      E_UseFileLogger,
      E_UseStdOutLogger
    };

    struct Opts
    {
      Opts()
        : logFile("log_output.txt")
        , loggerName("BSR")
        , loggerType(E_UseStdOutLogger)
      {
      
      }

      std::string logFile;
      std::string loggerName;
      int         loggerType;
    };

    Application(Opts const &);
    virtual ~Application();

    static Application * Instance();

    void PushLayer(Layer*);
    Layer * GetLayer(Layer::ID);

    void Run();
    void RequestQuit();

    bool NormalizeWindowCoords(int x, int y, float & x_out, float & y_out);

  private:

    Application(Application const &) = delete;
    Application & operator=(Application const &) = delete;

    void InitWindow();
    void EndFrame();

  private:

    static Application * s_instance;

  private:

    class PIMPL;
    PIMPL * m_pimpl;
  };

  // To be defined in CLIENT
  Application * CreateApplication();
}

#endif