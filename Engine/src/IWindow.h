//@group Interface

#ifndef EN_IWINDOW_H
#define EN_IWINDOW_H

#include <string>
#include "DgError.h"

namespace DgE
{
  struct WindowProps
  {
    WindowProps()
      : name("DgEngine")
      , width(1024)
      , height(768)
      , fullscreen(false)
    {}

    std::string name;
    unsigned width;
    unsigned height;
    bool fullscreen;
  };

  class IWindow
  {
  public:

    virtual ~IWindow(){};

    virtual void Update() = 0;

    virtual void SwapBuffers() = 0;
    virtual void SetVSync(bool) = 0;
    virtual bool IsVSync() const = 0;

    virtual bool IsInit() const = 0;
    virtual Dg::ErrorCode Init(WindowProps const & props = WindowProps()) = 0;
    virtual void Destroy() = 0;

    virtual void GetDimensions(int & w, int & h) = 0;

  private:

  };
}

#endif