//@group Layers

#ifndef EN_LAYER_UI_H
#define EN_LAYER_UI_H

#include "EngineMessages.h"
#include "Layer.h"

namespace Engine
{
  class Layer_UI : public Layer
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::UI))

    Layer_UI();
    ~Layer_UI();

    void HandleMessage(Message *) override;
    void HandleMessage(Message_GUI_MouseMove*);
    void HandleMessage(Message_GUI_MouseButtonDown *);
    void HandleMessage(Message_GUI_MouseButtonUp *);
    void HandleMessage(Message_GUI_KeyDown *);
    void HandleMessage(Message_GUI_KeyUp *);
    void HandleMessage(Message_GUI_MouseWheelDown *);
    void HandleMessage(Message_GUI_MouseWheelUp *);
    void HandleMessage(Message_GUI_Text *);

    void Update(float);
    void Render();

  private:

    float m_dt;
  };
}

#endif