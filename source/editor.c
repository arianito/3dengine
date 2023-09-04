#include "editor.h"

#include "camera.h"
#include "game.h"
#include "draw.h"
#include "debug.h"
#include "input.h"
#include "mathf.h"
#include "memory/alloc.h"

typedef enum
{
    NOT_BUSY = 0,
    ORBITING = 1,
    PANNING = 2,
    ZOOMING = 4,
} StatusEnum;

typedef struct
{
    Vec3 last_position;
    Rot last_rotation;
    Vec2 last_mousepos;
    Vec2 mousepos;
    Vec3 center;
    StatusEnum mode;
    float distance;
    float last_distance;
} EditorData;

static EditorData *editor;

void editor_init()
{
    editor = alloc_global(EditorData);
    clear(editor, sizeof(EditorData));

    editor->center = vec3_zero;
    editor->distance = vec3_dist(camera->position, editor->center);
}

void save_state()
{
    Vec3 forward = vec3_mulf(rot_forward(camera->rotation), editor->distance);
    editor->center = vec3_add(camera->position, forward);
    editor->mousepos = vec2_zero;
    editor->last_mousepos = vec2_zero;
    editor->last_distance = editor->distance;
    editor->last_position = camera->position;
    editor->last_rotation = camera->rotation;
}

inline void handle_input()
{
    if (!editor->mode && input_keypress(KEY_LEFT_ALT) && input_mousedown(MOUSE_RIGHT))
    {
        save_state();
        editor->mode = ZOOMING;
    }
    if (!editor->mode && input_keypress(KEY_LEFT_ALT) && input_mousedown(MOUSE_LEFT))
    {
        save_state();
        editor->mode = ORBITING;
    }
    if (!editor->mode && input_mousedown(MOUSE_RIGHT))
    {
        save_state();
        editor->mode = PANNING;
    }
    if (editor->mode && (input_mouseup(MOUSE_RIGHT) || input_mouseup(MOUSE_LEFT)))
        editor->mode = NOT_BUSY;

    if (input_keydown(KEY_F))
    {
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        editor->center = vec3_zero;
        camera->position = vec3_add(backward, editor->center);
    }

    if (editor->mode)
    {
        editor->mousepos.x += input->delta.x;
        editor->mousepos.y += input->delta.y;
    }
}

void editor_update()
{
    handle_input();
    draw_axis(vec3_zero, quat_identity, 10);

    Vec2 orbitingSensitivity = {0.25f, 0.25f};
    float panningSensitivity = 0.25f;
    float zoomingSensitivity = 0.25f;

    if (editor->mode == ORBITING)
    {
        float d = clamp(500.0f / editor->distance, 0.05f, 0.5f);
        float dy = (editor->mousepos.y - editor->last_mousepos.y) * orbitingSensitivity.y * d;
        float dx = (editor->mousepos.x - editor->last_mousepos.x) * orbitingSensitivity.x * d;

        camera->rotation.pitch = editor->last_rotation.pitch - dy;
        camera->rotation.yaw = editor->last_rotation.yaw + dx;

        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera_update();
        input_infinite();
    }
    else if (editor->mode == PANNING)
    {
        float d = clamp(editor->distance / 500.0f, 0.001f, 0.75f);
        float dx = (editor->mousepos.x - editor->last_mousepos.x) * panningSensitivity * d * -1.0f;
        float dy = (editor->mousepos.y - editor->last_mousepos.y) * panningSensitivity * d;

        Vec3 right = vec3_mulf(rot_right(camera->rotation), dx);
        Vec3 up = vec3_mulf(rot_up(camera->rotation), dy);

        camera->position = vec3_add(editor->last_position, vec3_add(up, right));

        input_infinite();
    }
    else if (editor->mode == ZOOMING)
    {
        float d = clamp(1000.0f / editor->distance, 0.001f, 0.5f);
        float y = (editor->mousepos.y * zoomingSensitivity) * d;
        editor->distance = fmaxf(editor->last_distance + y, 0.5f);
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        input_infinite();
    }

    camera_update();
}