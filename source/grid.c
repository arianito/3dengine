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

static Shader *shader;
enum
{
    size = 10 * UNIT_SCALE,
    d = 10,
    n = size / UNIT_SCALE,
    ne = (8 * n + 4),
};

static GLuint m_vaoIds[1];
static GLuint m_vboIds[1];
static Vertex base_grid[ne];

void grid_init()
{
    shader = shader_load("grid.vs", "grid.fs");
    glGenVertexArrays(2, m_vaoIds);
    glGenBuffers(2, m_vboIds);

    glBindVertexArray(m_vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(base_grid), base_grid, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // vertical
    Vertex a;
    Color c;
    float gap = (float)size / n;
    int k = 0, i;

    c = color_alpha(color_white, 0.1f);
    for (i = -n; i < n + 1; i++)
    {
        a.color = c;
        a.pos.x = i * gap;
        a.pos.y = -size;
        a.pos.z = 0;
        base_grid[k++] = a;
        a.pos.x = i * gap;
        a.pos.y = size;
        a.pos.z = 0;
        base_grid[k++] = a;
    }
    for (i = -n; i < n + 1; i++)
    {
        a.color = c;
        a.pos.x = -size;
        a.pos.y = i * gap;
        a.pos.z = 0;
        base_grid[k++] = a;
        a.pos.x = size;
        a.pos.y = i * gap;
        a.pos.z = 0;
        base_grid[k++] = a;
    }
}

void grid_terminate()
{
    glDeleteVertexArrays(2, m_vaoIds);
    glDeleteBuffers(2, m_vboIds);
    shader_destroy(shader);
}

void grid_render()
{
    Mat4 world;
    Vec3 t;
    int i, j;

    shader_begin(shader);
    shader_mat4(shader, "projection", &camera->projection);
    shader_mat4(shader, "view", &camera->view);

    glLineWidth(1);
    glBindVertexArray(m_vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(base_grid), base_grid);

    t.x = floorf(camera->position.x / UNIT_SCALE) * UNIT_SCALE;
    t.y = floorf(camera->position.y / UNIT_SCALE) * UNIT_SCALE;
    t.z = 0;

    world = mat4_origin(t);
    shader_mat4(shader, "world", &world);
    shader_float(shader, "alpha", 1.0f);
    glDrawArrays(GL_LINES, 0, sizeof(base_grid));
    for (i = -d; i < d + 1; i++)
    {
        for (j = -d; j < d + 1; j++)
        {
            t.x = i * UNIT_SCALE + floorf(camera->position.x / UNIT_SCALE) * UNIT_SCALE;
            t.y = j * UNIT_SCALE + floorf(camera->position.y / UNIT_SCALE) * UNIT_SCALE;
            t.z = 0;
            world = mat4_mul(mat4_scalef(0.1f), mat4_origin(t));
            shader_mat4(shader, "world", &world);
            shader_float(shader, "alpha", 0.5f);
            glDrawArrays(GL_LINES, 0, sizeof(base_grid));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_end();
}
