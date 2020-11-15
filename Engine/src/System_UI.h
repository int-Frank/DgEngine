//@group Systems

#ifndef EN_SYSTEM_UI_H
#define EN_SYSTEM_UI_H

#include "EngineMessages.h"
#include "System.h"
#include "UIFrame.h"

namespace Engine
{
  class System_UI : public System
  {
  public:

    ASSIGN_ID(static_cast<ID>(DefaultSystem::UI))
    
    System_UI();
    ~System_UI();

    void HandleMessage(Message *) override;

    void Update(float);
    void Render();



  private:

    void HandleMessage(Message_Window_Resized * a_pMsg);

  private:

    UIFrame m_frame;
    float m_dt;
  };
}

#endif