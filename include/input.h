#ifndef INPUT_H
#define INPUT_H

typedef enum
{
    NO_EVENT = 0,
    QUIT,
    MOUSE_MOTION,
    MOUSE_BUTTON,
    KEYBOARD_BUTTON
} event_type;

typedef enum
{
    KEYBOARD_0, // 0, backspace, delete
    KEYBOARD_1,
    KEYBOARD_2,
    KEYBOARD_3,
    KEYBOARD_4,
    KEYBOARD_5,
    KEYBOARD_6,
    KEYBOARD_7,
    KEYBOARD_8,
    KEYBOARD_9,
    KEYBOARD_O,
    KEYBOARD_S,
    KEYBOARD_PLUS,
    KEYBOARD_MINUS,
} keycode;

typedef struct
{
    event_type type;
    int x, y;
} mouse_event;

typedef struct
{
    event_type type;
    keycode key;
} keyboard_event;

typedef union
{
    event_type type;
    mouse_event mouse;
    keyboard_event keyboard;
} event;

event get_event(void);

#endif
