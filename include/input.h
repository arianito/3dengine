#pragma once

#include "mathf.h"


#define input_AXIS_VERTICAL 0
#define input_AXIS_HORIZONTAL 1

#define input_MOUSE_LEFT 0
#define input_MOUSE_RIGHT 1
#define input_MOUSE_MIDDLE 2

#define input_UNKNOWN -1

#define input_SPACE 32
#define input_APOSTROPHE 39
#define input_COMMA 44
#define input_MINUS 45
#define input_PERIOD 46
#define input_SLASH 47
#define input_0 48
#define input_1 49
#define input_2 50
#define input_3 51
#define input_4 52
#define input_5 53
#define input_6 54
#define input_7 55
#define input_8 56
#define input_9 57
#define input_SEMICOLON 59
#define input_EQUAL 61
#define input_A 65
#define input_B 66
#define input_C 67
#define input_D 68
#define input_E 69
#define input_F 70
#define input_G 71
#define input_H 72
#define input_I 73
#define input_J 74
#define input_K 75
#define input_L 76
#define input_M 77
#define input_N 78
#define input_O 79
#define input_P 80
#define input_Q 81
#define input_R 82
#define input_S 83
#define input_T 84
#define input_U 85
#define input_V 86
#define input_W 87
#define input_X 88
#define input_Y 89
#define input_Z 90
#define input_LEFT_BRACKET 91
#define input_BACKSLASH 92
#define input_RIGHT_BRACKET 93
#define input_GRAVE_ACCENT 96
#define input_WORLD_1 161
#define input_WORLD_2 162

#define input_ESCAPE 256
#define input_ENTER 257
#define input_TAB 258
#define input_BACKSPACE 259
#define input_INSERT 260
#define input_DELETE 261
#define input_RIGHT 262
#define input_LEFT 263
#define input_DOWN 264
#define input_UP 265
#define input_PAGE_UP 266
#define input_PAGE_DOWN 267
#define input_HOME 268
#define input_END 269
#define input_CAPS_LOCK 280
#define input_SCROLL_LOCK 281
#define input_NUM_LOCK 282
#define input_PRINT_SCREEN 283
#define input_PAUSE 284
#define input_F1 290
#define input_F2 291
#define input_F3 292
#define input_F4 293
#define input_F5 294
#define input_F6 295
#define input_F7 296
#define input_F8 297
#define input_F9 298
#define input_F10 299
#define input_F11 300
#define input_F12 301
#define input_F13 302
#define input_F14 303
#define input_F15 304
#define input_F16 305
#define input_F17 306
#define input_F18 307
#define input_F19 308
#define input_F20 309
#define input_F21 310
#define input_F22 311
#define input_F23 312
#define input_F24 313
#define input_F25 314
#define input_KP_0 320
#define input_KP_1 321
#define input_KP_2 322
#define input_KP_3 323
#define input_KP_4 324
#define input_KP_5 325
#define input_KP_6 326
#define input_KP_7 327
#define input_KP_8 328
#define input_KP_9 329
#define input_KP_DECIMAL 330
#define input_KP_DIVIDE 331
#define input_KP_MULTIPLY 332
#define input_KP_SUBTRACT 333
#define input_KP_ADD 334
#define input_KP_ENTER 335
#define input_KP_EQUAL 336
#define input_LEFT_SHIFT 340
#define input_LEFT_CONTROL 341
#define input_LEFT_ALT 342
#define input_LEFT_SUPER 343
#define input_RIGHT_SHIFT 344
#define input_RIGHT_CONTROL 345
#define input_RIGHT_ALT 346
#define input_RIGHT_SUPER 347
#define input_MENU 348

typedef struct
{
    Vec2 position;
    Vec2 delta;
} Input;

Input *input;

void input_init();
void input_update();
void input_terminate();
void input_infinite();

char input_keypress(short key);
char input_keyup(short key);
char input_keydown(short key);
char input_mousepress(short key);
char input_mouseup(short key);
char input_mousedown(short key);

float input_axis(int axis);