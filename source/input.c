
#include "input.h"

#include <GLFW/glfw3.h>

#include "alloc.h"
#include "game.h"

#define AXIS_SPEED 8

typedef struct
{
    float value;
    float velocity;
} InputAxis;

typedef struct
{
    int current;
    int prev;
} InputState;

typedef struct
{
    InputState keyState[__KEY_COUNT];
    InputState mouseState[__MOUSE_COUNT];
    InputAxis axes[__AXIS_COUNT];
} InputData;

static InputData *globalInput;

void update_axis(int ax, char low, char high)
{
    float to = 0.0f + (low ? -1.0f : 0.0f) + (high ? 1.0f : 0.0f);
    InputAxis *axis = &(globalInput->axes[ax]);
    axis->value = moveTowards(axis->value, to, AXIS_SPEED * time->deltaTime);
}

void input_init()
{
    globalInput = (InputData *)alloc_global(sizeof(InputData));
    clear(globalInput, sizeof(InputData));

    input = (Input *)alloc_global(sizeof(Input));
    input->position = vec2_zero;
    input->delta = vec2_zero;
}

void input_update()
{
    for (int i = 0; i < __KEY_COUNT; i++)
    {
        InputState *state = &(globalInput->keyState[i]);
        state->prev = state->current;
    }
    for (int i = 0; i < __MOUSE_COUNT; i++)
    {
        InputState *state = &(globalInput->mouseState[i]);
        state->prev = state->current;
    }

    double x, y;
    glfwGetCursorPos(game->window, &x, &y);

    input->delta.x = (float)x - input->position.x;
    input->delta.y = (float)y - input->position.y;
    input->position.x = (float)x;
    input->position.y = (float)y;

    update_axis(AXIS_HORIZONTAL,
                input_keypress(KEY_LEFT) || input_keypress(KEY_A),
                input_keypress(KEY_RIGHT) || input_keypress(KEY_D));

    update_axis(AXIS_VERTICAL,
                input_keypress(KEY_DOWN) || input_keypress(KEY_S),
                input_keypress(KEY_UP) || input_keypress(KEY_W));
}

void input_infinite()
{
    int pad = 10;
    char changed = 0;
    float width = game->width;
    float height = game->height;
    if (input->delta.x < 0 && input->position.x < pad)
    {
        input->position.x = width;
        input->delta.x = 0;
        changed = 1;
    }
    if (input->delta.x > 0 && input->position.x > width - pad)
    {
        input->position.x = 0;
        input->delta.x = 0;
        changed = 1;
    }
    if (input->delta.y < 0 && input->position.y < pad)
    {
        input->position.y = height;
        input->delta.y = 0;
        changed = 1;
    }
    if (input->delta.y > 0 && input->position.y > height - pad)
    {
        input->position.y = 0;
        input->delta.y = 0;
        changed = 1;
    }
    if (changed)
    {
        glfwSetCursorPos(game->window, input->position.x, input->position.y);
    }
}

void input_terminate()
{
}

int input_keypress(KeyEnum key)
{
    InputState *state = &globalInput->keyState[key];
    state->current = glfwGetKey(game->window, key);
    return state->current;
}

int input_keyup(KeyEnum key)
{
    InputState *state = &globalInput->keyState[key];
    state->current = glfwGetKey(game->window, key);
    return (!(state->current) && state->prev);
}

int input_keydown(KeyEnum key)
{
    InputState *state = &globalInput->keyState[key];
    state->current = glfwGetKey(game->window, key);
    return state->current && !state->prev;
}

int input_mousepress(MouseEnum key)
{
    InputState *state = &globalInput->mouseState[key];
    state->current = glfwGetMouseButton(game->window, key);
    return state->current;
}

int input_mouseup(MouseEnum key)
{
    InputState *state = &globalInput->mouseState[key];
    state->current = glfwGetMouseButton(game->window, key);
    return !state->current && state->prev;
}

int input_mousedown(MouseEnum key)
{
    InputState *state = &globalInput->mouseState[key];
    state->current = glfwGetMouseButton(game->window, key);
    return state->current && !state->prev;
}

float input_axis(AxisEnum a)
{
    InputAxis *state = &(globalInput->axes[a]);
    return state->value;
}
