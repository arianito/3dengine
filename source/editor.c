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
	Vec3 lastCamPos;
	Rot lastCamRot;
	Vec2 lastMousePos;
	Vec2 mousePos;
	Vec3 center;
	StatusEnum mode;
	float distance;
	float lastDistance;
	//
	float orbitingSensitivity;
	float panningSensitivity;
	float zoomingSensitivity;
} EditorData;

static EditorData *editor;

void editor_init()
{
	editor = alloc_global(EditorData);
	clear(editor, sizeof(EditorData));

	editor->center = vec3_zero;
	editor->distance = vec3_dist(camera->position, editor->center);
	editor->orbitingSensitivity = 0.25f;
	editor->panningSensitivity = 0.25f;
	editor->zoomingSensitivity = 0.25f;
}

void save_state()
{
	Vec3 forward = vec3_mulf(rot_forward(camera->rotation), editor->distance);
	editor->center = vec3_add(camera->position, forward);
	editor->mousePos = vec2_zero;
	editor->lastMousePos = vec2_zero;
	editor->lastDistance = editor->distance;
	editor->lastCamPos = camera->position;
	editor->lastCamRot = camera->rotation;
}

void editor_update()
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
		editor->mousePos.x += input->delta.x;
		editor->mousePos.y += input->delta.y;
	}

	if (editor->mode == ORBITING)
	{
		float d = clamp(500.0f / editor->distance, 0.05f, 0.5f);
		float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->orbitingSensitivity * d;
		float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->orbitingSensitivity * d;

		camera->rotation.pitch = editor->lastCamRot.pitch - dy;
		camera->rotation.yaw = editor->lastCamRot.yaw + dx;

		Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
		camera->position = vec3_add(backward, editor->center);
		camera_update();
		input_infinite();
	}
	else if (editor->mode == PANNING)
	{
		float d = clamp(editor->distance / 500.0f, 0.001f, 0.75f);
		float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->panningSensitivity * d * -1.0f;
		float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->panningSensitivity * d;

		Vec3 right = vec3_mulf(rot_right(camera->rotation), dx);
		Vec3 up = vec3_mulf(rot_up(camera->rotation), dy);

		camera->position = vec3_add(editor->lastCamPos, vec3_add(up, right));

		camera_update();
		input_infinite();
	}
	else if (editor->mode == ZOOMING)
	{
		float d = clamp(1000.0f / editor->distance, 0.001f, 0.5f);
		float y = (editor->mousePos.y * editor->zoomingSensitivity) * d;
		editor->distance = fmaxf(editor->lastDistance + y, 0.5f);
		Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
		camera->position = vec3_add(backward, editor->center);
		camera_update();
		input_infinite();
	}
}