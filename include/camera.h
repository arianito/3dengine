#pragma once
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

#include "mathf.h"
#include "game.h"
#include "input.h"

#include "mem/alloc.h"

typedef enum {
    VIEW_INITIAL = 0,
    VIEW_ORTHOGRAPHIC = 1 << 0,
    VIEW_BACK = 1 << 1,
    VIEW_FRONT = 1 << 2,
    VIEW_LEFT = 1 << 3,
    VIEW_RIGHT = 1 << 4,
    VIEW_BOTTOM = 1 << 5,
    VIEW_TOP = 1 << 6

} OrthoMode;

typedef struct {
    Vec3 position;
    Rot rotation;
    float zoom;

    Mat4 view;
    Mat4 projection;
    float fov;
    Mat4 viewProjection;
    int ortho;
} Camera;

extern Camera *camera;

static inline void camera_update() {
    float farPlane = 10000.0f;
    if (!(camera->ortho & VIEW_ORTHOGRAPHIC)) {
        camera->view = mat4_view(camera->position, camera->rotation);
        camera->projection = mat4_perspective(camera->fov, game->ratio, 0.0001f, farPlane);
    } else {
        Rot r = camera->rotation;
        r.pitch += 180;
        float height = camera->zoom * (camera->fov * 0.005556f);
        float width = height * game->ratio;
        camera->projection = mat4_orthographic(-width, width, height, -height, -farPlane, farPlane);
        camera->view = mat4_view(camera->position, r);
    }
    camera->viewProjection = mat4_mul(camera->view, camera->projection);
}

static inline void camera_init() {
    camera = alloc_global(Camera, sizeof(Camera));
    camera->rotation = rot(-15, 180, 0);
    Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -300);
    camera->position = vec3_add(backward, vec3_zero);
    camera->fov = 80.0f;
    camera->zoom = 300.0f;
    camera->ortho = VIEW_INITIAL;
    camera_update();
}

static inline char camera_worldToScreen(Vec3 p, Vec2 *out_screen) {
    Vec4 r = mat4_mulv4(camera->viewProjection, vec4(p.x, p.y, p.z, 1));
    if (r.w > 0) {
        float rhw = 1.0f / r.w;
        float rx = r.x * rhw;
        float ry = r.y * rhw;
        float nx = (rx / 2.f) + 0.5f;
        float ny = 1.f - (ry / 2.f) - 0.5f;

        *out_screen = vec2(nx * game->width, ny * game->height);
        return 1;
    }
    return 0;
}

static inline void camera_screenToWorld(Vec2 s, Vec3 *out_origin, Vec3 *out_dir) {
    Mat4 inv = mat4_inv(camera->viewProjection);
    float nx = s.x / game->width;
    float ny = s.y / game->height;
    float ssx = (nx - 0.5f) * 2.0f;
    float ssy = ((1.0f - ny) - 0.5f) * 2.0f;
    Vec4 rsp = vec4(ssx, ssy, 1.0f, 1.0f);
    Vec4 rep = vec4(ssx, ssy, 0.01f, 1.0f);

    Vec4 hrs = mat4_mulv4(inv, rsp);
    Vec4 hre = mat4_mulv4(inv, rep);
    Vec3 rsw = {hrs.x, hrs.y, hrs.z};
    Vec3 rew = {hre.x, hre.y, hre.z};

    if (hrs.w != 0.0f) {
        rsw.x /= hrs.w;
        rsw.y /= hrs.w;
        rsw.z /= hrs.w;
    }
    if (hre.w != 0.0f) {
        rew.x /= hre.w;
        rew.y /= hre.w;
        rew.z /= hre.w;
    }

    *out_origin = rsw;
    *out_dir = vec3_norm(vec3_sub(rew, rsw));
}