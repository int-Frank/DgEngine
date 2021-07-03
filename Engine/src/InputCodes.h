//@group Core

#ifndef EN_INPUTCODES_H
#define EN_INPUTCODES_H

#include <stdint.h>
#include "Utils.h"

#define GET_FULL_INPUT_CODE(code, event) ((static_cast<uint32_t>(code) << 16) | static_cast<uint32_t>(event))

#define KM_SHIFT (::Engine::KeyMod::KM_LSHIFT | ::Engine::KeyMod::KM_RSHIFT)
#define KM_CTRL (::Engine::KeyMod::KM_LCTRL | ::Engine::KeyMod::KM_RCTRL)
#define KM_ALT  (::Engine::KeyMod::KM_LALT | ::Engine::KeyMod::KM_RALT)
#define KM_GUI  (::Engine::KeyMod::KM_LGUI | ::Engine::KeyMod::KM_RGUI)

namespace Engine
{
  enum KeyMod : uint16_t
  {
    KM_LSHIFT   = BIT(1),
    KM_LCTRL    = BIT(2),
    KM_LALT     = BIT(3),
    KM_LGUI     = BIT(4),
    KM_RSHIFT   = BIT(5),
    KM_RCTRL    = BIT(6),
    KM_RALT     = BIT(7),
    KM_RGUI     = BIT(8),
    KM_NUM      = BIT(9),
    KM_CAPS     = BIT(10),
    KM_MODE     = BIT(11)
  };

#undef ITEM
#define INPUT_CODES \
  ITEM(UNKNOWN, 0)\
  ITEM(KEY_A, 0x04)\
  ITEM(KEY_B, 0x05)\
  ITEM(KEY_C, 0x06)\
  ITEM(KEY_D, 0x07)\
  ITEM(KEY_E, 0x08)\
  ITEM(KEY_F, 0x09)\
  ITEM(KEY_G, 0x0a)\
  ITEM(KEY_H, 0x0b)\
  ITEM(KEY_I, 0x0c)\
  ITEM(KEY_J, 0x0d)\
  ITEM(KEY_K, 0x0e)\
  ITEM(KEY_L, 0x0f)\
  ITEM(KEY_M, 0x10)\
  ITEM(KEY_N, 0x11)\
  ITEM(KEY_O, 0x12)\
  ITEM(KEY_P, 0x13)\
  ITEM(KEY_Q, 0x14)\
  ITEM(KEY_R, 0x15)\
  ITEM(KEY_S, 0x16)\
  ITEM(KEY_T, 0x17)\
  ITEM(KEY_U, 0x18)\
  ITEM(KEY_V, 0x19)\
  ITEM(KEY_W, 0x1a)\
  ITEM(KEY_X, 0x1b)\
  ITEM(KEY_Y, 0x1c)\
  ITEM(KEY_Z, 0x1d)\
  ITEM(KEY_1, 0x1e)\
  ITEM(KEY_2, 0x1f)\
  ITEM(KEY_3, 0x20)\
  ITEM(KEY_4, 0x21)\
  ITEM(KEY_5, 0x22)\
  ITEM(KEY_6, 0x23)\
  ITEM(KEY_7, 0x24)\
  ITEM(KEY_8, 0x25)\
  ITEM(KEY_9, 0x26)\
  ITEM(KEY_0, 0x27)\
  ITEM(KEY_ENTER, 0x28)\
  ITEM(KEY_ESC, 0x29)\
  ITEM(KEY_BACKSPACE, 0x2a)\
  ITEM(KEY_TAB, 0x2b)\
  ITEM(KEY_SPACE, 0x2c)\
  ITEM(KEY_MINUS, 0x2d)\
  ITEM(KEY_EQUAL, 0x2e)\
  ITEM(KEY_LEFTBRACE, 0x2f)\
  ITEM(KEY_RIGHTBRACE, 0x30)\
  ITEM(KEY_BACKSLASH, 0x31)\
  ITEM(KEY_HASHTILDE, 0x32)\
  ITEM(KEY_SEMICOLON, 0x33)\
  ITEM(KEY_APOSTROPHE, 0x34)\
  ITEM(KEY_GRAVE, 0x35)\
  ITEM(KEY_COMMA, 0x36)\
  ITEM(KEY_DOT, 0x37)\
  ITEM(KEY_SLASH, 0x38)\
  ITEM(KEY_CAPSLOCK, 0x39)\
  ITEM(KEY_F1, 0x3a)\
  ITEM(KEY_F2, 0x3b)\
  ITEM(KEY_F3, 0x3c)\
  ITEM(KEY_F4, 0x3d)\
  ITEM(KEY_F5, 0x3e)\
  ITEM(KEY_F6, 0x3f)\
  ITEM(KEY_F7, 0x40)\
  ITEM(KEY_F8, 0x41)\
  ITEM(KEY_F9, 0x42)\
  ITEM(KEY_F10, 0x43)\
  ITEM(KEY_F11, 0x44)\
  ITEM(KEY_F12, 0x45)\
  ITEM(KEY_SYSRQ, 0x46)\
  ITEM(KEY_SCROLLLOCK, 0x47)\
  ITEM(KEY_PAUSE, 0x48)\
  ITEM(KEY_INSERT, 0x49)\
  ITEM(KEY_HOME, 0x4a)\
  ITEM(KEY_PAGEUP, 0x4b)\
  ITEM(KEY_DELETE, 0x4c)\
  ITEM(KEY_END, 0x4d)\
  ITEM(KEY_PAGEDOWN, 0x4e)\
  ITEM(KEY_RIGHT, 0x4f)\
  ITEM(KEY_LEFT, 0x50)\
  ITEM(KEY_DOWN, 0x51)\
  ITEM(KEY_UP, 0x52)\
  ITEM(KEY_NUMLOCK, 0x53)\
  ITEM(KEY_KPSLASH, 0x54)\
  ITEM(KEY_KPASTERISK, 0x55)\
  ITEM(KEY_KPMINUS, 0x56)\
  ITEM(KEY_KPPLUS, 0x57)\
  ITEM(KEY_KPENTER, 0x58)\
  ITEM(KEY_KP1, 0x59)\
  ITEM(KEY_KP2, 0x5a)\
  ITEM(KEY_KP3, 0x5b)\
  ITEM(KEY_KP4, 0x5c)\
  ITEM(KEY_KP5, 0x5d)\
  ITEM(KEY_KP6, 0x5e)\
  ITEM(KEY_KP7, 0x5f)\
  ITEM(KEY_KP8, 0x60)\
  ITEM(KEY_KP9, 0x61)\
  ITEM(KEY_KP0, 0x62)\
  ITEM(KEY_KPDOT, 0x63)\
  ITEM(KEY_102ND, 0x64)\
  ITEM(KEY_COMPOSE, 0x65)\
  ITEM(KEY_KPEQUAL, 0x67)\
  ITEM(KEY_F13, 0x68)\
  ITEM(KEY_F14, 0x69)\
  ITEM(KEY_F15, 0x6a)\
  ITEM(KEY_F16, 0x6b)\
  ITEM(KEY_F17, 0x6c)\
  ITEM(KEY_F18, 0x6d)\
  ITEM(KEY_F19, 0x6e)\
  ITEM(KEY_F20, 0x6f)\
  ITEM(KEY_F21, 0x70)\
  ITEM(KEY_F22, 0x71)\
  ITEM(KEY_F23, 0x72)\
  ITEM(KEY_F24, 0x73)\
  ITEM(KEY_KPCOMMA, 0x85)\
  ITEM(KEY_RO, 0x87)\
  ITEM(KEY_KATAKANAHIRAGANA, 0x88)\
  ITEM(KEY_YEN, 0x89)\
  ITEM(KEY_HENKAN, 0x8a)\
  ITEM(KEY_MUHENKAN, 0x8b)\
  ITEM(KEY_KPJPCOMMA, 0x8c)\
  ITEM(KEY_HANGEUL, 0x90)\
  ITEM(KEY_HANJA, 0x91)\
  ITEM(KEY_KATAKANA, 0x92)\
  ITEM(KEY_HIRAGANA, 0x93)\
  ITEM(KEY_ZENKAKUHANKAKU, 0x94)\
  ITEM(KEY_KPLEFTPAREN, 0xb6)\
  ITEM(KEY_KPRIGHTPAREN, 0xb7)\
  ITEM(KEY_LEFTCTRL, 0xe0)\
  ITEM(KEY_LEFTSHIFT, 0xe1)\
  ITEM(KEY_LEFTALT, 0xe2)\
  ITEM(KEY_LEFTMETA, 0xe3)\
  ITEM(KEY_RIGHTCTRL, 0xe4)\
  ITEM(KEY_RIGHTSHIFT, 0xe5)\
  ITEM(KEY_RIGHTALT, 0xe6)\
  ITEM(KEY_RIGHTMETA, 0xe7)\
  ITEM(MOUSE_BUTTON_LEFT, 0xff)\
  ITEM(MOUSE_BUTTON_MIDDLE, 0x100)\
  ITEM(MOUSE_BUTTON_RIGHT, 0x101)\
  ITEM(MOUSE_BUTTON_X1, 0x102)\
  ITEM(MOUSE_BUTTON_X2, 0x103)\
  ITEM(MOUSE_WHEEL_UP, 0x104)\
  ITEM(MOUSE_WHEEL_DOWN, 0x105)\
  ITEM(MOUSE_MOTION, 0x106)\
  ITEM(TEXT, 0x107)\
  ITEM(COUNT, 0x0)\

  enum InputCode : uint16_t
  {
#undef ITEM
#define ITEM(name, val) IC_ ## name = val,
    INPUT_CODES
  };

  enum InputEvent : uint8_t
  {
    IE_VALUE_CHANGE,
    IE_BUTTON_UP,
    IE_BUTTON_DOWN
  };

  enum InputCodeIndex
  {
#undef ITEM
#define ITEM(name, val) nIC_##name,
    INPUT_CODES
  };

  extern uint16_t const InputKeyMap[nIC_COUNT + 1];

  //enum InputCode : uint16_t
  //{
  //  IC_UNKNOWN = 0,

  //  IC_KEY_A = 0x04, // Keyboard a and A
  //  IC_KEY_B = 0x05, // Keyboard b and B
  //  IC_KEY_C = 0x06, // Keyboard c and C
  //  IC_KEY_D = 0x07, // Keyboard d and D
  //  IC_KEY_E = 0x08, // Keyboard e and E
  //  IC_KEY_F = 0x09, // Keyboard f and F
  //  IC_KEY_G = 0x0a, // Keyboard g and G
  //  IC_KEY_H = 0x0b, // Keyboard h and H
  //  IC_KEY_I = 0x0c, // Keyboard i and I
  //  IC_KEY_J = 0x0d, // Keyboard j and J
  //  IC_KEY_K = 0x0e, // Keyboard k and K
  //  IC_KEY_L = 0x0f, // Keyboard l and L
  //  IC_KEY_M = 0x10, // Keyboard m and M
  //  IC_KEY_N = 0x11, // Keyboard n and N
  //  IC_KEY_O = 0x12, // Keyboard o and O
  //  IC_KEY_P = 0x13, // Keyboard p and P
  //  IC_KEY_Q = 0x14, // Keyboard q and Q
  //  IC_KEY_R = 0x15, // Keyboard r and R
  //  IC_KEY_S = 0x16, // Keyboard s and S
  //  IC_KEY_T = 0x17, // Keyboard t and T
  //  IC_KEY_U = 0x18, // Keyboard u and U
  //  IC_KEY_V = 0x19, // Keyboard v and V
  //  IC_KEY_W = 0x1a, // Keyboard w and W
  //  IC_KEY_X = 0x1b, // Keyboard x and X
  //  IC_KEY_Y = 0x1c, // Keyboard y and Y
  //  IC_KEY_Z = 0x1d, // Keyboard z and Z

  //  IC_KEY_1 = 0x1e, // Keyboard 1 and !
  //  IC_KEY_2 = 0x1f, // Keyboard 2 and @
  //  IC_KEY_3 = 0x20, // Keyboard 3 and #
  //  IC_KEY_4 = 0x21, // Keyboard 4 and $
  //  IC_KEY_5 = 0x22, // Keyboard 5 and %
  //  IC_KEY_6 = 0x23, // Keyboard 6 and ^
  //  IC_KEY_7 = 0x24, // Keyboard 7 and &
  //  IC_KEY_8 = 0x25, // Keyboard 8 and *
  //  IC_KEY_9 = 0x26, // Keyboard 9 and (
  //  IC_KEY_0 = 0x27, // Keyboard 0 and )

  //  IC_KEY_ENTER = 0x28,      // Keyboard Return (ENTER)
  //  IC_KEY_ESC = 0x29,        // Keyboard ESCAPE
  //  IC_KEY_BACKSPACE = 0x2a,  // Keyboard DELETE (Backspace)
  //  IC_KEY_TAB = 0x2b,        // Keyboard Tab
  //  IC_KEY_SPACE = 0x2c,      // Keyboard Spacebar
  //  IC_KEY_MINUS = 0x2d,      // Keyboard - and _
  //  IC_KEY_EQUAL = 0x2e,      // Keyboard = and +
  //  IC_KEY_LEFTBRACE = 0x2f,  // Keyboard [ and {
  //  IC_KEY_RIGHTBRACE = 0x30, // Keyboard ] and }
  //  IC_KEY_BACKSLASH = 0x31,  // Keyboard \ and |
  //  IC_KEY_HASHTILDE = 0x32,  // Keyboard Non-US # and ~
  //  IC_KEY_SEMICOLON = 0x33,  // Keyboard ; and :
  //  IC_KEY_APOSTROPHE = 0x34, // Keyboard ' and "
  //  IC_KEY_GRAVE = 0x35,      // Keyboard ` and ~
  //  IC_KEY_COMMA = 0x36,      // Keyboard , and <
  //  IC_KEY_DOT = 0x37,        // Keyboard . and >
  //  IC_KEY_SLASH = 0x38,      // Keyboard / and ?
  //  IC_KEY_CAPSLOCK = 0x39,   // Keyboard Caps Lock

  //  IC_KEY_F1 = 0x3a,   // Keyboard F1
  //  IC_KEY_F2 = 0x3b,   // Keyboard F2
  //  IC_KEY_F3 = 0x3c,   // Keyboard F3
  //  IC_KEY_F4 = 0x3d,   // Keyboard F4
  //  IC_KEY_F5 = 0x3e,   // Keyboard F5
  //  IC_KEY_F6 = 0x3f,   // Keyboard F6
  //  IC_KEY_F7 = 0x40,   // Keyboard F7
  //  IC_KEY_F8 = 0x41,   // Keyboard F8
  //  IC_KEY_F9 = 0x42,   // Keyboard F9
  //  IC_KEY_F10 = 0x43,  // Keyboard F10
  //  IC_KEY_F11 = 0x44,  // Keyboard F11
  //  IC_KEY_F12 = 0x45,  // Keyboard F12

  //  IC_KEY_SYSRQ = 0x46,      // Keyboard Print Screen
  //  IC_KEY_SCROLLLOCK = 0x47, // Keyboard Scroll Lock
  //  IC_KEY_PAUSE = 0x48,      // Keyboard Pause
  //  IC_KEY_INSERT = 0x49,     // Keyboard Insert
  //  IC_KEY_HOME = 0x4a,       // Keyboard Home
  //  IC_KEY_PAGEUP = 0x4b,     // Keyboard Page Up
  //  IC_KEY_DELETE = 0x4c,     // Keyboard Delete Forward
  //  IC_KEY_END = 0x4d,        // Keyboard End
  //  IC_KEY_PAGEDOWN = 0x4e,   // Keyboard Page Down
  //  IC_KEY_RIGHT = 0x4f,      // Keyboard Right Arrow
  //  IC_KEY_LEFT = 0x50,       // Keyboard Left Arrow
  //  IC_KEY_DOWN = 0x51,       // Keyboard Down Arrow
  //  IC_KEY_UP = 0x52,         // Keyboard Up Arrow

  //  IC_KEY_NUMLOCK = 0x53,    // Keyboard Num Lock and Clear
  //  IC_KEY_KPSLASH = 0x54,    // Keypad /
  //  IC_KEY_KPASTERISK = 0x55, // Keypad *
  //  IC_KEY_KPMINUS = 0x56,    // Keypad -
  //  IC_KEY_KPPLUS = 0x57,     // Keypad +
  //  IC_KEY_KPENTER = 0x58,    // Keypad ENTER
  //  IC_KEY_KP1 = 0x59,        // Keypad 1 and End
  //  IC_KEY_KP2 = 0x5a,        // Keypad 2 and Down Arrow
  //  IC_KEY_KP3 = 0x5b,        // Keypad 3 and PageDn
  //  IC_KEY_KP4 = 0x5c,        // Keypad 4 and Left Arrow
  //  IC_KEY_KP5 = 0x5d,        // Keypad 5
  //  IC_KEY_KP6 = 0x5e,        // Keypad 6 and Right Arrow
  //  IC_KEY_KP7 = 0x5f,        // Keypad 7 and Home
  //  IC_KEY_KP8 = 0x60,        // Keypad 8 and Up Arrow
  //  IC_KEY_KP9 = 0x61,        // Keypad 9 and Page Up
  //  IC_KEY_KP0 = 0x62,        // Keypad 0 and Insert
  //  IC_KEY_KPDOT = 0x63,      // Keypad . and Delete

  //  IC_KEY_102ND = 0x64,      // Keyboard Non-US \ and |
  //  IC_KEY_COMPOSE = 0x65,    // Keyboard Application
  //                            //IC_KEY_POWER = 0x66,      // Keyboard Power
  //  IC_KEY_KPEQUAL = 0x67,    // Keypad =

  //  IC_KEY_F13 = 0x68, // Keyboard F13
  //  IC_KEY_F14 = 0x69, // Keyboard F14
  //  IC_KEY_F15 = 0x6a, // Keyboard F15
  //  IC_KEY_F16 = 0x6b, // Keyboard F16
  //  IC_KEY_F17 = 0x6c, // Keyboard F17
  //  IC_KEY_F18 = 0x6d, // Keyboard F18
  //  IC_KEY_F19 = 0x6e, // Keyboard F19
  //  IC_KEY_F20 = 0x6f, // Keyboard F20
  //  IC_KEY_F21 = 0x70, // Keyboard F21
  //  IC_KEY_F22 = 0x71, // Keyboard F22
  //  IC_KEY_F23 = 0x72, // Keyboard F23
  //  IC_KEY_F24 = 0x73, // Keyboard F24

  //  IC_KEY_KPCOMMA = 0x85, // Keypad Comma

  //  IC_KEY_RO = 0x87,               // Keyboard International1
  //  IC_KEY_KATAKANAHIRAGANA = 0x88, // Keyboard International2
  //  IC_KEY_YEN = 0x89,              // Keyboard International3
  //  IC_KEY_HENKAN = 0x8a,           // Keyboard International4
  //  IC_KEY_MUHENKAN = 0x8b,         // Keyboard International5
  //  IC_KEY_KPJPCOMMA = 0x8c,        // Keyboard International6
  //  IC_KEY_HANGEUL = 0x90,          // Keyboard LANG1
  //  IC_KEY_HANJA = 0x91,            // Keyboard LANG2
  //  IC_KEY_KATAKANA = 0x92,         // Keyboard LANG3
  //  IC_KEY_HIRAGANA = 0x93,         // Keyboard LANG4
  //  IC_KEY_ZENKAKUHANKAKU = 0x94,   // Keyboard LANG5
  //  IC_KEY_KPLEFTPAREN = 0xb6,      // Keypad (
  //  IC_KEY_KPRIGHTPAREN = 0xb7,     // Keypad )

  //  IC_KEY_LEFTCTRL = 0xe0,   // Keyboard Left Control
  //  IC_KEY_LEFTSHIFT = 0xe1,  // Keyboard Left Shift
  //  IC_KEY_LEFTALT = 0xe2,    // Keyboard Left Alt
  //  IC_KEY_LEFTMETA = 0xe3,   // Keyboard Left GUI
  //  IC_KEY_RIGHTCTRL = 0xe4,  // Keyboard Right Control
  //  IC_KEY_RIGHTSHIFT = 0xe5, // Keyboard Right Shift
  //  IC_KEY_RIGHTALT = 0xe6,   // Keyboard Right Alt
  //  IC_KEY_RIGHTMETA = 0xe7,  // Keyboard Right GUI

  //  //----------------------------------------------------------------------------------
  //  // Mouse
  //  //----------------------------------------------------------------------------------
  //  IC_MOUSE_BUTTON_LEFT = 0xff,
  //  IC_MOUSE_BUTTON_MIDDLE,
  //  IC_MOUSE_BUTTON_RIGHT,
  //  IC_MOUSE_BUTTON_X1,
  //  IC_MOUSE_BUTTON_X2,
  //  IC_MOUSE_WHEEL_UP,
  //  IC_MOUSE_WHEEL_DOWN,
  //  IC_MOUSE_MOTION,

  //  IC_TEXT
  //};

}

#endif