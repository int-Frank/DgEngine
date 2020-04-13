//@group Layers

#ifndef EN_LAYER_APPLICATION_H
#define EN_LAYER_APPLICATION_H

#include "Memory.h"

#include "core_ErrorCodes.h"
#include "Layer.h"
#include "Memory.h"

namespace Engine
{
  class Layer_Application : public Layer
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::Application))

      Layer_Application();
    ~Layer_Application();

    void OnAttach();
    void OnDetach();

    void Update(float);

    void HandleMessage(Message *) override;
    void HandleMessage(Message_Command*);

  private:

  };
}


#endif