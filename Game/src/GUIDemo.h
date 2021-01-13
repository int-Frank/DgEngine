#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "System.h"
#include "Utils.h"

// We just use this system to generate the GUI
class GUIDemo : public Engine::System
{
public:

  MAKE_SYSTEM_DECL

  void OnAttach() override;
  void HandleMessage(Engine::Message * a_pMsg) override {}
  void OnDetach() override {}
  void Render() override {};
  void Update(float a_dt) override {}

private:

  void BuildGUI();
  void BindInput();

  Engine::Colour m_textColour;
};

#endif
