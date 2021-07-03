//@group Messages

#ifndef ENGINEMESSAGES_H
#define ENGINEMESSAGES_H

#include "Message.h"
#include "InputCodes.h"

#define TEXT_INPUT_TEXT_SIZE 32

// Smart list reduces the amount of code we have to write.
// Input messages never make it out of System_Input. This is where you convert them.
#undef ITEM
#define MESSAGE_LIST \
  ITEM(None, None) \
  ITEM(GUI_GoBack, GUI) \
  ITEM(GUI_Up, GUI) \
  ITEM(GUI_Down, GUI) \
  ITEM(GUI_Left, GUI) \
  ITEM(GUI_Right, GUI) \
  ITEM(GUI_Select, GUI) \
  ITEM(GUI_PointerDown, GUI) \
  ITEM(GUI_PointerUp, GUI) \
  ITEM(GUI_PointerMove, GUI) \
  ITEM(GUI_Text, GUI) \
  ITEM(Window_Shown, Window) \
  ITEM(Window_Hidden, Window) \
  ITEM(Window_Exposed, Window) \
  ITEM(Window_Moved, Window) \
  ITEM(Window_Resized, Window) \
  ITEM(Window_Minimized, Window) \
  ITEM(Window_Maximized, Window) \
  ITEM(Window_Restored, Window) \
  ITEM(Window_Enter, Window) \
  ITEM(Window_Leave, Window) \
  ITEM(Window_Focus_Gained, Window) \
  ITEM(Window_Focus_Lost, Window) \
  ITEM(Quit, None) \
  ITEM(Window_Take_Focus, Window) \
  ITEM(Input_Text, Input) \
  ITEM(Input_KeyUp, Input) \
  ITEM(Input_KeyDown, Input) \
  ITEM(Input_MouseButtonUp, Input) \
  ITEM(Input_MouseButtonDown, Input) \
  ITEM(Input_MouseWheelUp, Input) \
  ITEM(Input_MouseWheelDown, Input) \
  ITEM(Input_MouseMove, Input)

namespace Engine
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

  // TODO these shouldn't be messages, as they are only used by the System_Input.
  class Message_Input : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
  };

  class Message_Input_Text : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
    char text[TEXT_INPUT_TEXT_SIZE];
  };

  class Message_Input_MouseEvent : public Message
  {
    MESSAGE_HEADER

    InputCode code;
    InputEvent event;
    uint16_t   modState;
    int32_t    x;
    int32_t    y;
  };
}
#endif