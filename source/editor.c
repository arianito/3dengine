
#include "editor.h"

#include "camera.h"
#include "game.h"
#include "draw.h"
#include "debug.h"
#include "input.h"
#include "mathf.h"
#include "mem/alloc.h"

typedef enum {
    NOT_BUSY = 0,
    ORBITING = 1,
    PANNING = 2,
    ZOOMING = 4,
    FLYING = 8,
} StatusEnum;

typedef struct {
    Vec3 lastCamPos;
    Rot lastCamRot;
    Vec2 lastMousePos;
    Vec2 mousePos;
    Vec3 center;
    Vec3 focusPoint;
    float distance;
    float lastDistance;
    //
    float orbitingSensitivity;
    float panningSensitivity;
    float zoomingSensitivity;
    float flyingSpeed;
    //
    StatusEnum mode;
} EditorData;

static EditorData *editor;

void editor_init() {
    editor = alloc_global(EditorData, sizeof(EditorData));
    clear(editor, sizeof(EditorData));

    editor->center = vec3_zero;
    editor->distance = vec3_dist(camera->position, editor->center);
    editor->orbitingSensitivity = 0.25f;
    editor->panningSensitivity = 0.5f;
    editor->zoomingSensitivity = 0.5f;
    editor->focusPoint = vec3_zero;
}

void save_state() {
    Vec3 forward = vec3_mulf(rot_forward(camera->rotation), editor->distance);
    editor->center = vec3_add(camera->position, forward);
    editor->mousePos = vec2_zero;
    editor->lastMousePos = vec2_zero;
    editor->lastDistance = editor->distance;
    editor->lastCamPos = camera->position;
    editor->lastCamRot = camera->rotation;
    editor->flyingSpeed = 100.0f;

}

void editor_focus(Vec3 pos) {
    editor->focusPoint = pos;
    Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
    camera->zoom = editor->distance;
    editor->center = editor->focusPoint;
    camera->position = vec3_add(backward, editor->center);
    camera_update();
}

void editor_update() {
    if (!editor->mode && input_keypress(KEY_LEFT_CONTROL) && input_mousedown(MOUSE_MIDDLE)) {
        save_state();
        editor->mode = ZOOMING;
    }
    if (!editor->mode && input_keypress(KEY_LEFT_SHIFT) && input_mousedown(MOUSE_MIDDLE)) {
        save_state();
        editor->mode = PANNING;
    }
    if (!editor->mode && input_mousedown(MOUSE_MIDDLE)) {
        save_state();
        editor->mode = ORBITING;
    }
    if (input_mousedown(MOUSE_RIGHT)) {

        save_state();
        editor->mode = FLYING;
    }


    if (editor->mode && (input_mouseup(MOUSE_MIDDLE) || input_mouseup(MOUSE_RIGHT))) {
        editor->mode = NOT_BUSY;
    }
    if (editor->mode == FLYING) {

        float d = 0.25f;
        float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->orbitingSensitivity * d;
        float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->orbitingSensitivity * d;
        camera->rotation.pitch = editor->lastCamRot.pitch - dy;
        camera->rotation.yaw = editor->lastCamRot.yaw + dx;

        if (!(camera->ortho & VIEW_ORTHOGRAPHIC)) {
            editor->flyingSpeed = fmaxf(editor->flyingSpeed + input->wheel.y * 5.0f, 10.0f);

            float axisY = input_axis(AXIS_VERTICAL) * gameTime->deltaTime * editor->flyingSpeed;
            float axisX = input_axis(AXIS_HORIZONTAL) * gameTime->deltaTime * editor->flyingSpeed;

            Vec3 forward = vec3_mulf(rot_forward(camera->rotation), axisY);
            Vec3 right = vec3_mulf(rot_right(camera->rotation), axisX);

            camera->position = vec3_add(camera->position, vec3_add(forward, right));
            Vec3 fwd = vec3_mulf(rot_forward(camera->rotation), editor->distance);
            editor->center = vec3_add(camera->position, fwd);
        }

        camera_update();
        input_infinite();
    }

    if (editor->mode == ORBITING) {
        float d = clamp(500.0f / editor->distance, 0.05f, 0.5f);
        float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->orbitingSensitivity * d;
        float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->orbitingSensitivity * d;

        camera->rotation.pitch = editor->lastCamRot.pitch - dy;
        camera->rotation.yaw = editor->lastCamRot.yaw + dx;

        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                           VIEW_BOTTOM);
        camera_update();
        input_infinite();
    }

    if (editor->mode == PANNING) {
        float d = clamp(editor->distance / 500.0f, 0.001f, 0.75f);
        float sensitivity = editor->panningSensitivity;
        float dx = (editor->mousePos.x - editor->lastMousePos.x) * sensitivity * d * -1.0f;
        float dy = (editor->mousePos.y - editor->lastMousePos.y) * sensitivity * d;

        Vec3 right = vec3_mulf(rot_right(camera->rotation), dx);
        Vec3 up = vec3_mulf(rot_up(camera->rotation), dy);
        camera->position = vec3_add(editor->lastCamPos, vec3_add(up, right));
        Vec3 forward = vec3_mulf(rot_forward(camera->rotation), editor->distance);
        editor->center = vec3_add(camera->position, forward);
        camera_update();
        input_infinite();
    }

    if (editor->mode == ZOOMING) {
        float d = clamp(editor->lastDistance / 500.0f, 0.001f, 10.0f);
        float y = (editor->mousePos.y * editor->zoomingSensitivity) * d;
        editor->distance = fmaxf(editor->lastDistance + y, 1.0f);
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->zoom = editor->distance;
        camera_update();
        input_infinite();
    }

    if (!editor->mode && !near0(input->wheel.y)) {
        float d = clamp(editor->distance / 500.0f, 0.001f, 10.0f);
        editor->distance = fmaxf(editor->distance - input->wheel.y * d * 20.0f, 0.5f);
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->zoom = editor->distance;
        camera_update();
    }

    if (input_keydown(KEY_1)) {
        camera->ortho ^= VIEW_ORTHOGRAPHIC;
        camera_update();
    }

    if (input_keydown(KEY_2)) {
        if (camera->ortho & (VIEW_LEFT | VIEW_RIGHT))
            editor->center.y = 0;
        else if (camera->ortho & (VIEW_TOP | VIEW_BOTTOM))
            editor->center.z = 0;
        camera->ortho |= VIEW_ORTHOGRAPHIC;

        if (camera->ortho & VIEW_FRONT) {

            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_BACK;
            camera->rotation = rot(0, 0, 0);
        } else {
            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_FRONT;
            camera->rotation = rot(0, 180, 0);
        }

        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->zoom = editor->distance;
        camera_update();
    }

    if (input_keydown(KEY_3)) {
        if (camera->ortho & (VIEW_BACK | VIEW_FRONT))
            editor->center.x = 0;
        else if (camera->ortho & (VIEW_TOP | VIEW_BOTTOM))
            editor->center.z = 0;
        camera->ortho |= VIEW_ORTHOGRAPHIC;

        if (camera->ortho & VIEW_RIGHT) {

            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_LEFT;
            camera->rotation = rot(0, 90, 0);
        } else {
            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_RIGHT;
            camera->rotation = rot(0, -90, 0);
        }

        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->zoom = editor->distance;
        camera_update();
    }
    if (input_keydown(KEY_4)) {
        if (camera->ortho & (VIEW_BACK | VIEW_FRONT)) {
            editor->center.x = 0;
            editor->center.z = 0;
        } else if (camera->ortho & (VIEW_LEFT | VIEW_RIGHT)) {

            editor->center.y = 0;
            editor->center.z = 0;
        }
        camera->ortho |= VIEW_ORTHOGRAPHIC;
        if (camera->ortho & VIEW_TOP) {
            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_BOTTOM;
            camera->rotation = rot(90, 0, 0);
        } else {
            camera->ortho &= ~(VIEW_FRONT | VIEW_RIGHT | VIEW_TOP | VIEW_BACK | VIEW_LEFT |
                               VIEW_BOTTOM);
            camera->ortho |= VIEW_TOP;
            camera->rotation = rot(-90, 0, 0);
        }
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->position = vec3_add(backward, editor->center);
        camera->zoom = editor->distance;
        camera_update();
    }

    if (input_keydown(KEY_F)) {
        Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
        camera->zoom = editor->distance;
        editor->center = editor->focusPoint;
        camera->position = vec3_add(backward, editor->center);
        camera_update();
    }

    if (editor->mode) {
        editor->mousePos.x += input->delta.x;
        editor->mousePos.y += input->delta.y;
    }

    Vec2 pos = vec2(12, 0);
    debug_color(color_white);
    debug_origin(vec2(0, 0));

    if (camera->ortho & VIEW_BACK && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Back");
    else if (camera->ortho & VIEW_FRONT && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Front");
    else if (camera->ortho & VIEW_LEFT && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Left");
    else if (camera->ortho & VIEW_RIGHT && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Right");
    else if (camera->ortho & VIEW_BOTTOM && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Bottom");
    else if (camera->ortho & VIEW_TOP && (camera->ortho & VIEW_ORTHOGRAPHIC))
        debug_stringf(pos, "Top");
    else if (camera->ortho & VIEW_ORTHOGRAPHIC)
        debug_stringf(pos, "Ortho");
    else
        debug_stringf(pos, "Perspective");

    pos.x = game->width - pos.x;
    debug_origin(vec2(1, 0));
    debug_stringf(pos, "%d", game->fps);

    draw_axis(vec3_zero, 10, quat_identity);
}