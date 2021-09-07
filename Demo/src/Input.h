#ifndef INPUT_H
#define INPUT_H

#include "System.h"

using namespace DgE;

// Handles bindings, translates input to camera movement
class Input : public System
{
public:

  MAKE_SYSTEM_DECL

  Input();
  ~Input();

  void OnAttach() override;
  void HandleMessage(Message * a_pMsg) override;
  void HandleMessage(Message_Window_Resized * a_pMsg);
  void OnDetach() override;
  void Render() override;
  void Update(float a_dt) override;

private:

  class PIMPL;
  PIMPL * m_pimpl;
};

#endif