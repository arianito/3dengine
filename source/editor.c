#include "editor.h"

#include "camera.h"
#include "game.h"
#include "draw.h"
#include "debug.h"
#include "input.h"
#include "mathf.h"
#include "mem.h"

static char buff[128];

static Vec3 last_position;
static Rot last_rotation;
static Vec2 last_mousepos;
static Vec2 mousepos;
static Vec3 center;
static float distance = 100;
static float last_distance = 0;

enum busy_mode
{
    not_busy = 0,
    busy_orbiting = 1,
    busy_panning = 2,
    busy_zooming = 4,
};
static char busy = 0;

void editor_init()
{
    center = vec3_zero;
    distance = vec3_dist(camera->position, center);
}

void save_state()
{
    Vec3 forward = vec3_mulf(rot_forward(camera->rotation), distance);
    center = vec3_add(camera->position, forward);
    mousepos = vec2_zero;
    last_mousepos = vec2_zero;
    last_distance = distance;
    last_position = camera->position;
    last_rotation = camera->rotation;
}

inline void handle_input()
{
    if (!busy && input_keypress(input_LEFT_ALT) && input_mousedown(input_MOUSE_RIGHT))
    {
        save_state();
        busy = busy_zooming;
    }
    if (!busy && input_keypress(input_LEFT_ALT) && input_mousedown(input_MOUSE_LEFT))
    {
        save_state();
        busy = busy_orbiting;
    }
    if (!busy && input_mousedown(input_MOUSE_RIGHT))
    {
        save_state();
        busy = busy_panning;
    }
    if (busy && (input_mouseup(input_MOUSE_RIGHT) || input_mouseup(input_MOUSE_LEFT)))
        busy = not_busy;

    if (input_keydown(input_F))
    {
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -distance);
        center = vec3_zero;
        camera->position = vec3_add(backward, center);
    }

    if (busy)
    {
        mousepos.x += input->delta.x;
        mousepos.y += input->delta.y;
    }
}
void editor_update()
{
    draw_axis(vec3_zero, quat_identity, 10);

    handle_input();

    Vec2 orbiting_sensitivity = {0.25f, 0.25f};
    float panning_sensitivity = 0.25f;
    float zooming_sensitivity = 0.5f;

    draw_cube(vec3_zero, color_yellow, 40);

    if (busy == busy_orbiting)
    {
        float dy = (mousepos.y - last_mousepos.y) * orbiting_sensitivity.y;
        float dx = (mousepos.x - last_mousepos.x) * orbiting_sensitivity.x;

        camera->rotation.pitch = last_rotation.pitch - dy;
        camera->rotation.yaw = last_rotation.yaw + dx;

        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -distance);
        camera->position = vec3_add(backward, center);
        camera_update();
        input_infinite();
    }
    else if (busy == busy_panning)
    {
        float dx = (mousepos.x - last_mousepos.x) * panning_sensitivity * -1.0f;
        float dy = (mousepos.y - last_mousepos.y) * panning_sensitivity;

        Vec3 right = vec3_mulf(rot_right(camera->rotation), dx);
        Vec3 up = vec3_mulf(rot_up(camera->rotation), dy);

        camera->position = vec3_add(last_position, vec3_add(up, right));

        input_infinite();
    }
    else if (busy == busy_zooming)
    {
        float y = mousepos.y * zooming_sensitivity;
        distance = fmaxf(last_distance + y, EPSILON);
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -distance);
        camera->position = vec3_add(backward, center);
        input_infinite();
    }

    camera_update();
}

void editor_terminate()
{
}