#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "System.h"
#include "Utils.h"
#include "common.h"

// We just use this system to generate the GUI
class GUIDemo : public DgE::System
{
public:

  MAKE_SYSTEM_DECL

  void OnAttach() override;
  void HandleMessage(DgE::Message * a_pMsg) override {}
  void OnDetach() override {}
  void Render() override {};
  void Update(float a_dt) override {}

private:

  void BuildGUI();
  void BindInput();

  DgE::Colour m_textColour;
};

#endif
