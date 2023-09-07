/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/
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
	FLYING = 8,
} StatusEnum;

typedef struct
{
	Vec3 lastCamPos;
	Rot lastCamRot;
	Vec2 lastMousePos;
	Vec2 mousePos;
	Vec3 center;
	float distance;
	float lastDistance;
	//
	float orbitingSensitivity;
	float panningSensitivity;
	float zoomingSensitivity;
	float lastOrbit;
	//
	StatusEnum mode;
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
	editor->lastOrbit = -1;
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
	int leftAlt = input_keypress(KEY_LEFT_ALT);
	char allowed = time->time - editor->lastOrbit < 0.25f;

	if (!editor->mode && leftAlt && !allowed && input_mousedown(MOUSE_RIGHT))
	{
		save_state();
		editor->mode = ZOOMING;
	}
	if (!editor->mode && input_mousedown(MOUSE_RIGHT))
	{
		save_state();
		editor->mode = PANNING;
	}
	if (!editor->mode && leftAlt && input_mousedown(MOUSE_LEFT))
	{
		save_state();
		editor->mode = ORBITING;
		editor->lastOrbit = time->time;
	}

	if (input_keyup(KEY_LEFT_ALT))
	{
		editor->lastOrbit = -1;
	}

	if (editor->mode && (input_mouseup(MOUSE_RIGHT) || input_mouseup(MOUSE_LEFT)))
	{
		editor->mode = NOT_BUSY;
	}

	if (input_keydown(KEY_F))
	{
		Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
		editor->center = vec3_zero;
		camera->position = vec3_add(backward, editor->center);
		camera_update();
	}

	if (editor->mode == FLYING)
	{
		float axisY = input_axis(AXIS_VERTICAL);
		float axisX = input_axis(AXIS_HORIZONTAL);

		float d = 0.25f;
		float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->orbitingSensitivity * d;
		float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->orbitingSensitivity * d;
		camera->rotation.pitch = editor->lastCamRot.pitch - dy;
		camera->rotation.yaw = editor->lastCamRot.yaw + dx;

		Vec3 forward = vec3_mulf(rot_forward(camera->rotation), axisY);
		Vec3 right = vec3_mulf(rot_right(camera->rotation), axisX);

		camera->position = vec3_add(camera->position, vec3_add(forward, right));
		editor->lastOrbit = -1;
		camera_update();
		input_infinite();
	}
	else if (editor->mode == ORBITING)
	{
		float d = clamp(500.0f / editor->distance, 0.05f, 0.5f);
		float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->orbitingSensitivity * d;
		float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->orbitingSensitivity * d;

		camera->rotation.pitch = editor->lastCamRot.pitch - dy;
		camera->rotation.yaw = editor->lastCamRot.yaw + dx;

		Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -editor->distance);
		camera->position = vec3_add(backward, editor->center);
		editor->lastOrbit = time->time;
		camera_update();
		input_infinite();
	}
	else if (editor->mode == PANNING)
	{
		if (input_keydown(KEY_W) || input_keydown(KEY_S) || input_keydown(KEY_A) || input_keydown(KEY_D))
		{
			save_state();
			editor->mode = FLYING;
			return;
		}
		float d = clamp(editor->distance / 500.0f, 0.001f, 0.75f);
		float dx = (editor->mousePos.x - editor->lastMousePos.x) * editor->panningSensitivity * d * -1.0f;
		float dy = (editor->mousePos.y - editor->lastMousePos.y) * editor->panningSensitivity * d;

		Vec3 right = vec3_mulf(rot_right(camera->rotation), dx);
		Vec3 up = vec3_mulf(rot_up(camera->rotation), dy);
		camera->position = vec3_add(editor->lastCamPos, vec3_add(up, right));
		editor->lastOrbit = time->time;
		Vec3 forward = vec3_mulf(rot_forward(camera->rotation), editor->distance);
		editor->center = vec3_add(camera->position, forward);
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
	if (allowed)
	{
		float r = editor->distance / 50.0f;
		draw_circleXY(editor->center, color_blue, r, 12);
		draw_circleXZ(editor->center, color_green, r, 12);
		draw_circleYZ(editor->center, color_red, r, 12);
		draw_axis(editor->center, quat_identity, r);
	}
	if (editor->mode)
	{
		editor->mousePos.x += input->delta.x;
		editor->mousePos.y += input->delta.y;
	}

	draw_axis(vec3_zero, quat_identity, 10);
}