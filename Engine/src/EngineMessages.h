//@group Messages

#ifndef ENGINEMESSAGES_H
#define ENGINEMESSAGES_H

#include "Message.h"

#define TEXT_INPUT_TEXT_SIZE 32

//Smart list reduces the amount of code we have to write
#undef ITEM
#define MESSAGE_LIST \
  ITEM(None, None) \
  ITEM(GoBack, None) \
  ITEM(Window_Shown, Window) \
  ITEM(Window_Hidden, Window) \
  ITEM(Window_Exposed, Window) \
  ITEM(Window_Moved, Window) \
  ITEM(Window_Resized, Window) \
  ITEM(Window_Size_Changed, Window) \
  ITEM(Window_Minimized, Window) \
  ITEM(Window_Maximized, Window) \
  ITEM(Window_Restored, Window) \
  ITEM(Window_Enter, Window) \
  ITEM(Window_Leave, Window) \
  ITEM(Window_Focus_Gained, Window) \
  ITEM(Window_Focus_Lost, Window) \
  ITEM(Window_Close, Window) \
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

  MESSAGE_CLASS_HEADER(None) };
  MESSAGE_CLASS_HEADER(GoBack) };

  MESSAGE_CLASS_HEADER(Window_Shown) };
  MESSAGE_CLASS_HEADER(Window_Hidden) };
  MESSAGE_CLASS_HEADER(Window_Exposed) };

  MESSAGE_CLASS_HEADER(Window_Moved)
    int32_t   x;
    int32_t   y;
  };

  MESSAGE_CLASS_HEADER(Window_Resized)
    int32_t   w;
    int32_t   h;
  };

  MESSAGE_CLASS_HEADER(Window_Size_Changed)
    int32_t   w;
    int32_t   h;
  };

  MESSAGE_CLASS_HEADER(Window_Minimized) };
  MESSAGE_CLASS_HEADER(Window_Maximized) };
  MESSAGE_CLASS_HEADER(Window_Restored) };
  MESSAGE_CLASS_HEADER(Window_Enter) };
  MESSAGE_CLASS_HEADER(Window_Leave) };
  MESSAGE_CLASS_HEADER(Window_Focus_Gained) };
  MESSAGE_CLASS_HEADER(Window_Focus_Lost) };
  MESSAGE_CLASS_HEADER(Window_Close) };
  MESSAGE_CLASS_HEADER(Window_Take_Focus) };

  MESSAGE_CLASS_HEADER(Input_Text)
    char text[TEXT_INPUT_TEXT_SIZE];
  };

  MESSAGE_CLASS_HEADER(Input_KeyUp)
    uint32_t  keyCode;
    uint16_t   modState;
  };

  MESSAGE_CLASS_HEADER(Input_KeyDown)
    uint32_t  keyCode;
    uint16_t   modState;
  };

  MESSAGE_CLASS_HEADER(Input_MouseButtonUp)
    uint32_t  button;
    int32_t    x;
    int32_t    y;
  };

  MESSAGE_CLASS_HEADER(Input_MouseButtonDown)
    uint32_t  button;
    int32_t    x;
    int32_t    y;
  };

  MESSAGE_CLASS_HEADER(Input_MouseWheelUp) };
  MESSAGE_CLASS_HEADER(Input_MouseWheelDown) };

  MESSAGE_CLASS_HEADER(Input_MouseMove)
    int32_t x;
    int32_t y;
  };
}
#endif