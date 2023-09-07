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

#include "memory/alloc.h"

typedef struct
{

    Vec3 position;
    Rot rotation;
    Mat4 view;
    Mat4 projection;
    float fov;
    Mat4 viewProjection;
} Camera;

Camera *camera;

inline void camera_update()
{
    camera->view = mat4_view(camera->position, camera->rotation);
    camera->projection = mat4_perspective(camera->fov, game->ratio, 1.0f, 5000.0f);
    camera->viewProjection = mat4_mul(camera->view, camera->projection);
}

inline void camera_init()
{
    camera = alloc_global(Camera);

    camera->rotation = rot(-15, 45, 0);
    Vec3 backward = vec3_mulf(rot_forward(camera->rotation), -200);
	backward.y += 120;
    camera->position = vec3_add(backward, vec3_zero);
    camera->fov = 90.0f;

    camera_update();
}

inline void camera_terminate()
{
}

inline char camera_worldToScreen(Vec3 p, Vec2 *out_screen)
{
    Vec4 r = mat4_mulv4(camera->viewProjection, vec4(p.x, p.y, p.z, 1));
    if (r.w > 0)
    {
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

inline void camera_screenToWorld(Vec2 s, Vec3 *out_origin, Vec3 *out_dir)
{
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

    if (hrs.w != 0.0f)
    {
        rsw.x /= hrs.w;
        rsw.y /= hrs.w;
        rsw.z /= hrs.w;
    }
    if (hre.w != 0.0f)
    {
        rew.x /= hre.w;
        rew.y /= hre.w;
        rew.z /= hre.w;
    }

    *out_origin = rsw;
    *out_dir = vec3_norm(vec3_sub(rew, rsw));
}