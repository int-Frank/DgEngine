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

    void Update(float);
    void Render();

  private:

    float m_dt;
  };
}

#endif