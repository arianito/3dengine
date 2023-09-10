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
#include "camera.h"

#define BUFFER_OFFSET(x) ((const void *)(x))

enum
{
	quad_n = 4096,
};

typedef struct
{
	Character buffer[quad_n * 6];
	Shader shader;
	GLuint vaoIds[1];
	GLuint vboIds[1];
	int quads;
} DrawData;

static DrawData *drawData;

void debug_init()
{
	drawData = alloc_global(DrawData);
	clear(drawData, sizeof(DrawData));

	drawData->shader = shader_load("shaders/debug.vs", "shaders/debug.fs");
	glGenVertexArrays(1, drawData->vaoIds);
	glGenBuffers(1, drawData->vboIds);

	glBindVertexArray(drawData->vaoIds[0]);
	glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(drawData->buffer), drawData->buffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Character), BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void debug_render()
{
	if (drawData->quads == 0)
		return;

	shader_begin(drawData->shader);

	Mat4 projection = mat4_orthographic(0, game->width, game->height, 0, -1, 1);
	shader_mat4(drawData->shader, "projection", &projection);

	glBindVertexArray(drawData->vaoIds[0]);
	glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(drawData->buffer), drawData->buffer);
	glDrawArrays(GL_TRIANGLES, 0, drawData->quads * 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader_end();

	drawData->quads = 0;
}

void debug_terminate()
{
	glDeleteVertexArrays(1, drawData->vaoIds);
	glDeleteBuffers(1, drawData->vboIds);
	shader_destroy(drawData->shader);
}

void debug_string(const char *s, Vec2 pos)
{
	drawData->quads += stb_easy_font_print(pos.x, pos.y, s, 0, drawData->buffer + drawData->quads * 6, sizeof(drawData->buffer) - drawData->quads * 6);
}

void debug_string3d(const char *s, Vec3 pos)
{
	Vec2 scr;
	camera_worldToScreen(pos, &scr);
	drawData->quads += stb_easy_font_print(scr.x, scr.y, s, 0, drawData->buffer + drawData->quads * 6, sizeof(drawData->buffer) - drawData->quads * 6);
}
