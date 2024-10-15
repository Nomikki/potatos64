#include <drivers/keyboard.h>
#include <stdio.h>

typedef struct
{
  uint8_t keycode;
  uint8_t status_mask;
} key_event;

key_event keyboard_buffer[MAX_KEYB_BUFFER_SIZE];
uint8_t buf_position = 0;
uint8_t current_state = 0;
uint8_t current_modifiers = 0;

void init_keyboard()
{
  current_state = NORMAL_STATE;
}

char keyboard_get_printable_char(key_event key)
{
  const bool shifted = !(key.status_mask & (1 << SHIFT_MASK));
  // printf("%b : %b\n", key.status_mask, shifted);
  // return 0;

  switch (key.keycode)
  {
  case KEY_SECTION_SIGN:
    return shifted ? '§' : '½';
  case KEY_NUM1:
    return shifted ? '1' : '!';
  case KEY_NUM2:
    return shifted ? '2' : '"';
  case KEY_NUM3:
    return shifted ? '3' : '#';
  case KEY_NUM4:
    return shifted ? '4' : '¤';
  case KEY_NUM5:
    return shifted ? '5' : '%';
  case KEY_NUM6:
    return shifted ? '6' : '&';
  case KEY_NUM7:
    return shifted ? '7' : '/';
  case KEY_NUM8:
    return shifted ? '8' : '(';
  case KEY_NUM9:
    return shifted ? '9' : ')';
  case KEY_NUM0:
    return shifted ? '0' : '=';
  case KEY_PLUS:
    return shifted ? '+' : '?';
  case KEY_BACKSPACE:
    return KEY_BACKSPACE;

  case KEY_Q:
    return shifted ? 'q' : 'Q';
  case KEY_W:
    return shifted ? 'w' : 'W';
  case KEY_E:
    return shifted ? 'e' : 'E';
  case KEY_R:
    return shifted ? 'r' : 'R';
  case KEY_T:
    return shifted ? 't' : 'T';
  case KEY_Y:
    return shifted ? 'y' : 'Y';
  case KEY_U:
    return shifted ? 'u' : 'U';
  case KEY_I:
    return shifted ? 'i' : 'I';
  case KEY_O:
    return shifted ? 'o' : 'O';
  case KEY_P:
    return shifted ? 'p' : 'P';
  case KEY_UMLAUT:
    return shifted ? '¨' : '^';

  case KEY_A:
    return shifted ? 'a' : 'A';
  case KEY_S:
    return shifted ? 's' : 'S';
  case KEY_D:
    return shifted ? 'd' : 'D';
  case KEY_F:
    return shifted ? 'f' : 'F';
  case KEY_G:
    return shifted ? 'g' : 'G';
  case KEY_H:
    return shifted ? 'h' : 'H';
  case KEY_J:
    return shifted ? 'j' : 'J';
  case KEY_K:
    return shifted ? 'k' : 'K';
  case KEY_L:
    return shifted ? 'l' : 'L';
  case KEY_ASTERISK:
    return shifted ? '\'' : '*';

  case KEY_LESS_OR_GREATER:
    return shifted ? '<' : '>';
  case KEY_Z:
    return shifted ? 'z' : 'Z';
  case KEY_X:
    return shifted ? 'x' : 'X';
  case KEY_C:
    return shifted ? 'c' : 'C';
  case KEY_V:
    return shifted ? 'v' : 'V';
  case KEY_B:
    return shifted ? 'b' : 'B';
  case KEY_N:
    return shifted ? 'n' : 'N';
  case KEY_M:
    return shifted ? 'm' : 'M';
  case KEY_COMMA:
    return shifted ? ',' : ';';
  case KEY_PERIOD:
    return shifted ? '.' : ':';
  case KEY_DASH:
    return shifted ? '-' : '_';
  case KEY_SPACE:
    return shifted ? ' ' : ' ';
  case KEY_ENTER:
    return shifted ? '\n' : '\n';

  default:
    return 0;
  }
}

void keyboard_driver_irq_handler()
{
  // read byte from the keyboard data port.
  uint8_t scancode = inportb(KB_DATA_PORT);
  if (scancode == 0xE0)
  {
    current_state = PREFIX_STATE;
    return;
  }
  if (current_state == PREFIX_STATE)
  {
    current_state = NORMAL_STATE;
  }

  if (scancode == KEY_SHIFTL || scancode == KEY_SHIFTR)
  {
    current_modifiers |= 1 << SHIFT_MASK;
  }
  if (scancode == KEY_SHIFTL + RELEASE || scancode == KEY_SHIFTR + RELEASE)
  {
    current_modifiers &= ~(1 << SHIFT_MASK);
  }

  keyboard_buffer[buf_position].keycode = scancode;
  keyboard_buffer[buf_position].status_mask = current_modifiers;

  // printf("%i", scancode);

  char key = keyboard_get_printable_char(keyboard_buffer[buf_position]);
  if (key != 0)
    printf("%c", key);

  buf_position = (buf_position + 1) % MAX_KEYB_BUFFER_SIZE;
}
