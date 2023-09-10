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

	Rot r2 = camera->rotation;
	r2.pitch += 90.0f;
	Rot r = camera->rotation;
	r.pitch += 90.0f;
	r = rot_snap(r, 90);

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

	if (!(camera->ortho & VIEW_ORTHOGRAPHIC) || rot_nearEq(r2, r))
	{

		if (camera->ortho & (VIEW_FRONT | VIEW_BACK))
		{
			t.x = floorf(camera->position.z / UNIT_SCALE) * UNIT_SCALE;
			t.y = floorf(camera->position.y / UNIT_SCALE) * UNIT_SCALE;
			t.z = 0;
		}
		else if (camera->ortho & (VIEW_LEFT | VIEW_RIGHT))
		{
			t.x = floorf(camera->position.z / UNIT_SCALE) * UNIT_SCALE;
			t.y = floorf(-camera->position.x / UNIT_SCALE) * UNIT_SCALE;
			t.z = 0;
		}
	}
	world = mat4_origin(t);
	if ((camera->ortho & VIEW_ORTHOGRAPHIC) && rot_nearEq(r2, r))
		world = mat4_mul(world, rot_matrix(r, vec3_zero));

	shader_mat4(gridData->shader, "world", &world);
	shader_float(gridData->shader, "alpha", 1.0f);
	glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));

	for (i = -d; i < d + 1; i++)
	{
		for (j = -d; j < d + 1; j++)
		{
			Vec3 t2 = t;
			t2.x += i * (UNIT_SCALE);
			t2.y += j * (UNIT_SCALE);
			world = mat4_mul(mat4_scalef(1.0f / d), mat4_origin(t2));
			if ((camera->ortho & VIEW_ORTHOGRAPHIC) && rot_nearEq(r2, r))
				world = mat4_mul(world, rot_matrix(r, vec3_zero));
			shader_mat4(gridData->shader, "world", &world);
			shader_float(gridData->shader, "alpha", 0.3f);
			glDrawArrays(GL_LINES, 0, sizeof(gridData->vertices));
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader_end();
}
