//@group Layers

#ifndef EN_LAYER_WINDOW_H
#define EN_LAYER_WINDOW_H

#include "Memory.h"

#include "core_ErrorCodes.h"
#include "Layer.h"
#include "Memory.h"

namespace Engine
{
  class IWindow;

  class Layer_Window : public Layer
  {

  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::Window))

    Layer_Window(Ref<IWindow>);
    ~Layer_Window();

    void OnAttach();
    void OnDetach();

    void Update(float);

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Window_Shown*);
    void HandleMessage(Message_Window_Hidden *);
    void HandleMessage(Message_Window_Exposed *);
    void HandleMessage(Message_Window_Moved *);
    void HandleMessage(Message_Window_Resized *);
    void HandleMessage(Message_Window_Size_Changed *);
    void HandleMessage(Message_Window_Minimized *);
    void HandleMessage(Message_Window_Maximized *);
    void HandleMessage(Message_Window_Restored *);
    void HandleMessage(Message_Window_Enter *);
    void HandleMessage(Message_Window_Leave *);
    void HandleMessage(Message_Window_Focus_Gained *);
    void HandleMessage(Message_Window_Focus_Lost *);
    void HandleMessage(Message_Window_Close *);
    void HandleMessage(Message_Window_Take_Focus *);

  private:

    Ref<IWindow> m_pWindow;
  };
}


#endif