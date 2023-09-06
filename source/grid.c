#include "draw.h"

#define BUFFER_OFFSET(x) ((const void *)(x))
#define UNIT_SCALE 100

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "shader.h"
#include "game.h"
#include "camera.h"
#include "mathf.h"

enum
{
    n = 10,
    d = 10,
    size = n * UNIT_SCALE,
    ne = (8 * n + 4),
};

typedef struct
{
    Shader shader;
    GLuint vaoIds[1];
    GLuint vboIds[1];
    Vertex vertices[ne];
} GridData;

static GridData *gridData;

void grid_init()
{

    gridData = alloc_global(GridData);
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
    float gap = (float)size / n;

    c = color_alpha(color_white, 0.1f);
    for (i = -n; i < n + 1; i++)
    {
        a.color = c;
        a.pos.x = i * gap;
        a.pos.y = -size;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
        a.pos.x = i * gap;
        a.pos.y = size;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
    }
    for (i = -n; i < n + 1; i++)
    {
        a.color = c;
        a.pos.x = -size;
        a.pos.y = i * gap;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
        a.pos.x = size;
        a.pos.y = i * gap;
        a.pos.z = 0;
        gridData->vertices[k++] = a;
    }
}

void grid_terminate()
{
    glDeleteVertexArrays(2, gridData->vaoIds);
    glDeleteBuffers(2, gridData->vboIds);
    shader_destroy(gridData->shader);
}

void grid_render()
{
    Mat4 world;
    Vec3 t;
    int i, j;

    shader_begin(gridData->shader);
    shader_mat4(gridData->shader, "projection", &camera->projection);
    shader_mat4(gridData->shader, "view", &camera->view);

    glLineWidth(1);
    glBindVertexArray(gridData->vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, gridData->vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gridData->vertices), gridData->vertices);

    t.x = floorf(camera->position.x / UNIT_SCALE) * UNIT_SCALE;
    t.y = floorf(camera->position.y / UNIT_SCALE) * UNIT_SCALE;
    t.z = 0;

    world = mat4_origin(t);
    shader_mat4(gridData->shader, "world", &world);
    shader_float(gridData->shader, "alpha", 1.0f);
    glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));
    for (i = -d; i < d + 1; i++)
    {
        for (j = -d; j < d + 1; j++)
        {
            t.x = i * UNIT_SCALE + floorf(camera->position.x / UNIT_SCALE) * UNIT_SCALE;
            t.y = j * UNIT_SCALE + floorf(camera->position.y / UNIT_SCALE) * UNIT_SCALE;
            t.z = 0;
            world = mat4_mul(mat4_scalef(1.0f / d), mat4_origin(t));
            shader_mat4(gridData->shader, "world", &world);
            shader_float(gridData->shader, "alpha", 0.5f);
            glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_end();
}
