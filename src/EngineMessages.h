//@group Messages

#ifndef ENGINEMESSAGES_H
#define ENGINEMESSAGES_H

#include "Message.h"
#include "InputCodes.h"

#define TEXT_INPUT_TEXT_SIZE 32

namespace DgE
{
  //-----------------------------------------------------------------------------------
  // Message Classes
  //-----------------------------------------------------------------------------------

  class Message_None : public Message { MESSAGE_HEADER }; // eg Esc...
  class Message_GUI_GoBack : public Message { MESSAGE_HEADER };
  class Message_GUI_Up : public Message { MESSAGE_HEADER };     // eg arrow keys, numpad arrows, joystick controls...
  class Message_GUI_Down : public Message { MESSAGE_HEADER };
  class Message_GUI_Left : public Message { MESSAGE_HEADER };
  class Message_GUI_Right : public Message { MESSAGE_HEADER };
  class Message_GUI_Select : public Message { MESSAGE_HEADER }; // eg Enter, xBox controller 'A'

  // eg mouse click, joystick pointer...
  class Message_GUI_PointerDown : public Message
  {
    MESSAGE_HEADER
    uint32_t context; // eg main select, secondary select (left click, right click)
    int32_t x;
    int32_t y;
  };

  // eg mouse click, joystick pointer...
  class Message_GUI_PointerUp : public Message
  {
    MESSAGE_HEADER
    uint32_t context; // eg main select, secondary select (left click, right click)
    int32_t x;
    int32_t y;
  };

  class Message_GUI_PointerMove : public Message
  {
    MESSAGE_HEADER
    void ConsumeHover(); // Call this instead of setting the message flag to 'Handled'
    int32_t x;
    int32_t y;
  };

  class Message_GUI_Text : public Message
  {
    MESSAGE_HEADER
    char text[TEXT_INPUT_TEXT_SIZE];
  };

  class Message_Window_Shown : public Message { MESSAGE_HEADER };
  class Message_Window_Hidden : public Message { MESSAGE_HEADER };
  class Message_Window_Exposed : public Message { MESSAGE_HEADER };

  class Message_Window_Moved : public Message
  {
    MESSAGE_HEADER
    int32_t   x;
    int32_t   y;
  };

  class Message_Window_Resized : public Message
  {
    MESSAGE_HEADER
    int32_t   w;
    int32_t   h;
  };
  
  class Message_Window_Minimized : public Message { MESSAGE_HEADER };
  class Message_Window_Maximized : public Message { MESSAGE_HEADER };
  class Message_Window_Restored : public Message { MESSAGE_HEADER };
  class Message_Window_Enter : public Message { MESSAGE_HEADER };
  class Message_Window_Leave : public Message { MESSAGE_HEADER };
  class Message_Window_Focus_Gained : public Message { MESSAGE_HEADER };
  class Message_Window_Focus_Lost : public Message { MESSAGE_HEADER };
  class Message_Quit : public Message { MESSAGE_HEADER };
  class Message_Window_Take_Focus : public Message { MESSAGE_HEADER };

  // These are generated from raw input
  class Message_Input_Key : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
  };

  class Message_Input_Mouse : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
    int32_t    x;
    int32_t    y;
  };

  class Message_Input_Text : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
    char text[TEXT_INPUT_TEXT_SIZE];
  };
}
#endif