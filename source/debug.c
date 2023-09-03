#include "debug.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "game.h"
#include "shader.h"
#include "mathf.h"
#include "font.h"

#define BUFFER_OFFSET(x) ((const void *)(x))
#define VERTEX_SIZE 20
#define CIRCLE_SEGMENTS 18

static Shader *shader;

static GLuint m_vaoIds[1];
static GLuint m_vboIds[1];

static Character buffer[2048];
static int num_quads = 0;


void debug_init()
{
    shader = shader_load("shaders/debug.vs", "shaders/debug.fs");
    glGenVertexArrays(1, m_vaoIds);
    glGenBuffers(1, m_vboIds);

    glBindVertexArray(m_vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Character), BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void debug_render()
{
    if (num_quads == 0)
        return;

    shader_begin(shader);

    Mat4 projection = mat4_orthographic(0, game->width, game->height, 0, -1, 1);
    shader_mat4(shader, "projection", &projection);

    glBindVertexArray(m_vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
    glDrawArrays(GL_TRIANGLES, 0, num_quads * 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_end();

    num_quads = 0;
}

void debug_terminate()
{
    glDeleteVertexArrays(1, m_vaoIds);
    glDeleteBuffers(1, m_vboIds);
    shader_destroy(shader);
}

void debug_string(char *s, Vec2 pos)
{
    num_quads = stb_easy_font_print(pos.x, pos.y, s, 0, buffer, sizeof(buffer));
}
