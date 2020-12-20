//@group Systems

#ifndef EN_SYSTEM_UI_H
#define EN_SYSTEM_UI_H

#include "EngineMessages.h"
#include "System.h"
#include "GUI_Container.h"

namespace Engine
{
  class System_GUI : public System
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultSystem::UI))
    
    System_GUI(int windowW, int windowH);
    ~System_GUI();

    void HandleMessage(Message *) override;

    void Update(float);
    void Render();

    void ClearFrame();
    void AddWidget(GUI::Widget *);

  private:

    void HandleMessage(Message_Window_Resized * a_pMsg);

  private:

    GUI::Container * m_pScreen;
    float m_dt;
  };
}

#endif