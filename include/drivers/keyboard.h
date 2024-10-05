#ifndef __drivers__keyboard__
#define __drivers__keyboard__

#include <types.h>

#define KB_DATA_PORT 0x60
#define KB_READ_STATUS 0x64
#define KB_WRITE_COMMAND 0x64

#define MAX_KEYB_BUFFER_SIZE 255

#define NORMAL_STATE 0
#define PREFIX_STATE 1

#define CTRL_MASK 1
#define ALT_MASK 2
#define SHIFT_MASK 3

#define RELEASE 128

enum kernel_scancodes
{
  KEY_ESCAPE = 1,
  KEY_NUM1 = 2,
  KEY_NUM2 = 3,
  KEY_NUM3 = 4,
  KEY_NUM4 = 5,
  KEY_NUM5 = 6,
  KEY_NUM6 = 7,
  KEY_NUM7 = 8,
  KEY_NUM8 = 9,
  KEY_NUM9 = 10,
  KEY_NUM0 = 11,
  KEY_PLUS = 12,
  KEY_BACKSLASH = 13,
  KEY_BACKSPACE = 14,
  KEY_TAB = 15,
  KEY_Q = 16,
  KEY_W = 17,
  KEY_E = 18,
  KEY_R = 19,
  KEY_T = 20,
  KEY_Y = 21,
  KEY_U = 22,
  KEY_I = 23,
  KEY_O = 24,
  KEY_P = 25,

  KEY_UMLAUT = 27,
  KEY_ENTER = 28,
  KEY_CTRL = 29,
  KEY_A = 30,
  KEY_S = 31,
  KEY_D = 32,
  KEY_F = 33,
  KEY_G = 34,
  KEY_H = 35,
  KEY_J = 36,
  KEY_K = 37,
  KEY_L = 38,
  KEY_OO = 39,
  KEY_AA = 40,
  KEY_SECTION_SIGN = 41,
  KEY_SHIFTL = 42,
  KEY_ASTERISK = 43,
  KEY_Z = 44,
  KEY_X = 45,
  KEY_C = 46,
  KEY_V = 47,
  KEY_B = 48,
  KEY_N = 49,
  KEY_M = 50,
  KEY_COMMA = 51,
  KEY_PERIOD = 52,
  KEY_DASH = 53,
  KEY_SHIFTR = 54,
  KEY_NUMPAD_ASTERISK = 55,
  KEY_ALT = 56,
  KEY_SPACE = 57,
  KEY_CAPSLOCK = 58,
  KEY_F1 = 59,
  KEY_F2 = 60,
  KEY_F3 = 61,
  KEY_F4 = 62,
  KEY_F5 = 63,
  KEY_F6 = 64,
  KEY_F7 = 65,
  KEY_F8 = 66,
  KEY_F9 = 67,
  KEY_F10 = 68,
  KEY_F11 = 87,
  KEY_F12 = 88,
  KEY_NUMLOCK = 69,
  KEY_LESS_OR_GREATER = 86,
  // TODO
};

extern void keyboard_init();
extern void keyboard_driver_irq_handler();

#endif