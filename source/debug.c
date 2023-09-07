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

static Shader shader;

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
