#include "input.h"
#include "malloc.h"
#include "game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define AXIS_SPEED 2

int keyboard_state[350];
int last_keyboard_state[350];

int mouse_state[3];
int last_mouse_state[8];
float axises[2];

void update_axis(int ax, char low, char high)
{
    float to = 0.0f + (low ? -1.0f : 0.0f) + (high ? 1.0f : 0.0f);
    axises[ax] = lerp01(axises[ax], to, AXIS_SPEED * game->delta_time);
}

void input_init()
{
    input = (Input *)malloc(sizeof(Input));
    input->position = vec2_zero;
    input->delta = vec2_zero;
}

void input_update()
{
    for (int i = 0; i < 350; i++)
        last_keyboard_state[i] = keyboard_state[i];
    for (int i = 0; i < 3; i++)
        last_mouse_state[i] = mouse_state[i];

    double x, y;
    glfwGetCursorPos(game->window, &x, &y);
    input->delta.x = (float)x - input->position.x;
    input->delta.y = (float)y - input->position.y;
    input->position.x = (float)x;
    input->position.y = (float)y;

    update_axis(input_AXIS_HORIZONTAL, input_keypress(input_LEFT) || input_keypress(input_A), input_keypress(input_RIGHT) || input_keypress(input_D));
    update_axis(input_AXIS_VERTICAL, input_keypress(input_DOWN) || input_keypress(input_S), input_keypress(input_UP) || input_keypress(input_W));
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
    free(input);
}

char input_keypress(short key)
{
    keyboard_state[key] = glfwGetKey(game->window, key);
    return keyboard_state[key];
}

char input_keyup(short key)
{
    keyboard_state[key] = glfwGetKey(game->window, key);
    return !keyboard_state[key] && last_keyboard_state[key];
}

char input_keydown(short key)
{
    keyboard_state[key] = glfwGetKey(game->window, key);
    return keyboard_state[key] && !last_keyboard_state[key];
}

char input_mousepress(short key)
{
    mouse_state[key] = glfwGetMouseButton(game->window, key);
    return mouse_state[key];
}

char input_mouseup(short key)
{
    mouse_state[key] = glfwGetMouseButton(game->window, key);
    return !mouse_state[key] && last_mouse_state[key];
}

char input_mousedown(short key)
{
    mouse_state[key] = glfwGetMouseButton(game->window, key);
    return mouse_state[key] && !last_mouse_state[key];
}

float input_axis(int axis)
{
    return axises[axis];
}
