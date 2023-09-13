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
#define BUFFER_OFFSET(x) ((const void *)(x))
#define UNIT_SCALE 10

#include <glad/glad.h>
#include "shader.h"
#include "camera.h"
#include "mathf.h"

enum {
    n = 100,
    d = 10,
    size = n * UNIT_SCALE,
    ne = (8 * n + 4),
};

typedef struct {
    Shader shader;
    GLuint vaoIds[1];
    GLuint vboIds[1];
    Vertex vertices[ne];
} GridData;

static GridData *gridData;

void grid_init() {

    gridData = alloc_global(GridData, sizeof(GridData));
    clear(gridData, sizeof(GridData));

    gridData->shader = shader_load("shaders/grid.vs", "shaders/grid.fs");
    glGenVertexArrays(2, gridData->vaoIds);
    glGenBuffers(2, gridData->vboIds);

    glBindVertexArray(gridData->vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, gridData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridData->vertices), gridData->vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Vertex a;
    Color c;
    int k = 0, i;
    float gap = (float) size / n;

    c = color_alpha(color_white, 0.1f);
    for (i = -n; i < n + 1; i++) {
        a.color = c;
        a.pos.x = (float) i * gap;
        a.pos.y = -size;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
        a.pos.x = (float) i * gap;
        a.pos.y = size;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
    }
    for (i = -n; i < n + 1; i++) {
        a.color = c;
        a.pos.x = -size;
        a.pos.y = (float) i * gap;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
        a.pos.x = size;
        a.pos.y = (float) i * gap;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
    }
}

void grid_terminate() {
    glDeleteVertexArrays(2, gridData->vaoIds);
    glDeleteBuffers(2, gridData->vboIds);
    shader_destroy(gridData->shader);
}

void grid_render() {

    Rot r2 = camera->rotation;
    r2.pitch += 90.0f;
    Rot r3 = camera->rotation;
    r3.pitch += 90.0f;
    r3 = rot_snap(r3, 90);

    Mat4 world1;
    Mat4 world2;
    Vec3 t = vec3_zero;

    shader_begin(gridData->shader);
    shader_mat4(gridData->shader, "projection", &camera->projection);
    shader_mat4(gridData->shader, "view", &camera->view);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(1);
    glBindVertexArray(gridData->vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, gridData->vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gridData->vertices), gridData->vertices);
    char isOrtho = (char) (camera->ortho & VIEW_ORTHOGRAPHIC);

    t.x = camera->position.x;
    t.y = camera->position.y;
    t.z = 0;

    float flt = 0.25f;
    flt = (sind(fabsf(fmodf(camera->rotation.pitch, 180) ) )) * flt;
    if (!isOrtho) {
        flt = flt * 0.25f + 0.25f;
    }
    if (isOrtho && rot_nearEq(r2, r3)) {

        float r = camera->ortho & (VIEW_TOP) ? 1.0f : -1.0f;
        t.x *= r;
        if (camera->ortho & (VIEW_FRONT | VIEW_BACK)) {
            r = camera->ortho & (VIEW_BACK) ? 1.0f : -1.0f;
            t.x = camera->position.z;
            t.y = r * camera->position.y;
            t.z = 0;
        } else if (camera->ortho & (VIEW_LEFT | VIEW_RIGHT)) {
            r = camera->ortho & (VIEW_RIGHT) ? 1.0f : -1.0f;
            t.x = camera->position.z;
            t.y = r * camera->position.x;
            t.z = 0;
        }
        flt = 0.25f;
        world1 = mat4_mul(mat4_origin(vec3_snap(t, UNIT_SCALE * d)), rot_matrix(r3, vec3_zero));
        world2 = mat4_mul(mat4_scalef(d), world1);
    } else {
        world1 = mat4_origin(vec3_snap(t, UNIT_SCALE * d));
        world2 = mat4_mul(mat4_scalef(d), world1);
    }
    float factor = (1.0f - clamp01((camera->zoom - UNIT_SCALE * 100) / (UNIT_SCALE * 10.0f)));

    shader_mat4(gridData->shader, "world", &world1);
    shader_float(gridData->shader, "alpha", flt * factor);
    glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));

    glLineWidth(2);
    shader_mat4(gridData->shader, "world", &world2);
    shader_float(gridData->shader, "alpha", flt * 2);
    glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));

    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_end();
}
