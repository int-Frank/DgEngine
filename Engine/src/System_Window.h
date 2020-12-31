//@group Systems

#ifndef EN_SYSTEM_WINDOW_H
#define EN_SYSTEM_WINDOW_H

#include "Memory.h"

#include "DgError.h"
#include "System.h"
#include "Memory.h"

namespace Engine
{
  class IWindow;

  class System_Window : public System
  {

  public:

    MAKE_SYSTEM_DECL

    System_Window(IWindow *);
    ~System_Window();

    void OnAttach();
    void OnDetach();

    void Update(float);

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Window_Shown*);
    void HandleMessage(Message_Window_Hidden *);
    void HandleMessage(Message_Window_Exposed *);
    void HandleMessage(Message_Window_Moved *);
    void HandleMessage(Message_Window_Resized *);
    void HandleMessage(Message_Window_Minimized *);
    void HandleMessage(Message_Window_Maximized *);
    void HandleMessage(Message_Window_Restored *);
    void HandleMessage(Message_Window_Enter *);
    void HandleMessage(Message_Window_Leave *);
    void HandleMessage(Message_Window_Focus_Gained *);
    void HandleMessage(Message_Window_Focus_Lost *);
    void HandleMessage(Message_Quit *);
    void HandleMessage(Message_Window_Take_Focus *);

  private:

    IWindow * m_pWindow;
  };
}


#endif