//@group Layers

#ifndef EN_LAYER_CONSOLE_H
#define EN_LAYER_CONSOLE_H

#include "EngineMessages.h"
#include "Layer.h"

//Perhaps we can remove this from the Engine. It really belongs in the Game project.
namespace Engine
{
  class Layer_Console : public Layer
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultLayer::Console))

    Layer_Console();
    ~Layer_Console();

    void HandleMessage(Message*) override;
    void HandleMessage(Message_Command*);

#undef ITEM
#define ITEM(TYPE, CATEGORY) void HandleMessage(Message_##TYPE *);
    MESSAGE_LIST;

    void Update(float);

  };
}

#endif