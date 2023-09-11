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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "mathf.h"

#include "shader.h"
#include "camera.h"
#include "input.h"

#define BUFFER_OFFSET(x) ((const void *)(x))
#define VERTEX_SIZE 4

enum
{
	types_n = 3,
	object_count = 100000,
};

typedef struct
{
	GLuint vaoIds[types_n];
	GLuint vboIds[types_n];
	unsigned int types[types_n];
	unsigned int counter[types_n];
	Shader shader;
	Vertex vertices[types_n][object_count];
} DrawData;

static DrawData *drawData;

void draw_init()
{
	printf("--------------------------- %zu \n", sizeof(DrawData));
	drawData = alloc_global(DrawData, sizeof(DrawData));
	clear(drawData, sizeof(DrawData));

	drawData->types[0] = GL_POINTS;
	drawData->types[1] = GL_LINES;
	drawData->types[2] = GL_TRIANGLES;

	drawData->counter[0] = 0;
	drawData->counter[1] = 0;
	drawData->counter[2] = 0;

	drawData->shader = shader_load("shaders/draw.vs", "shaders/draw.fs");
	glGenVertexArrays(types_n, drawData->vaoIds);
	glGenBuffers(types_n, drawData->vboIds);

	for (int i = 0; i < types_n; i++)
	{
		glBindVertexArray(drawData->vaoIds[i]);
		glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(drawData->vertices[i]), drawData->vertices[i], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(32));

		drawData->counter[i] = 0;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_render()
{
	shader_begin(drawData->shader);
	shader_mat4(drawData->shader, "projection", &camera->projection);
	shader_mat4(drawData->shader, "view", &camera->view);

	glLineWidth(1);

	for (int i = 0; i < types_n; i++)
	{
		int count = drawData->counter[i];
		if (count == 0)
			continue;

		glBindVertexArray(drawData->vaoIds[i]);
		glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[i]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Vertex), drawData->vertices[i]);
		glDrawArrays(drawData->types[i], 0, count);

		drawData->counter[i] = 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader_end();
}

void draw_terminate()
{
	glDeleteVertexArrays(types_n, drawData->vaoIds);
	glDeleteBuffers(types_n, drawData->vboIds);
	shader_destroy(drawData->shader);
}

inline void add_vertex(int type, Vertex v)
{
	if (drawData->counter[type] == object_count)
		return;
	int count = drawData->counter[type];
	drawData->vertices[type][count] = v;
	drawData->counter[type]++;
}

void draw_point(Vec3 pos, Color c, float size)
{
	Vertex v;
	v.color = c;
	v.pos = pos;
	v.size = size;
	add_vertex(0, v);
}

void draw_line(Vec3 a, Vec3 b, Color c)
{
	Vertex va;
	va.color = c;
	va.pos = a;
	add_vertex(1, va);
	va.pos = b;
	add_vertex(1, va);
}

void draw_axis(Vec3 a, Quat q, float s)
{
	Vec3 forward = vec3_add(a, vec3_mulf(quat_forward(q), s));
	Vec3 right = vec3_add(a, vec3_mulf(quat_right(q), s));
	Vec3 up = vec3_add(a, vec3_mulf(quat_up(q), s));

	Vertex va;
	va.size = VERTEX_SIZE;

	va.pos = a;
	va.color = color_red;
	add_vertex(1, va);
	va.pos = forward;
	add_vertex(1, va);
	add_vertex(0, va);

	va.pos = a;
	va.color = color_green;
	add_vertex(1, va);
	va.pos = right;
	add_vertex(1, va);
	add_vertex(0, va);

	va.pos = a;
	va.color = color_blue;
	add_vertex(1, va);
	va.pos = up;
	add_vertex(1, va);
	add_vertex(0, va);
}

void draw_axisRot(Vec3 a, Rot r, float s)
{
	Vec3 forward = vec3_add(a, vec3_mulf(rot_forward(r), s));
	Vec3 right = vec3_add(a, vec3_mulf(rot_right(r), s));
	Vec3 up = vec3_add(a, vec3_mulf(rot_up(r), s));

	Vertex va;
	va.size = VERTEX_SIZE;

	va.pos = a;
	va.color = color_red;
	add_vertex(1, va);
	va.pos = forward;
	add_vertex(1, va);
	add_vertex(0, va);

	va.pos = a;
	va.color = color_green;
	add_vertex(1, va);
	va.pos = right;
	add_vertex(1, va);
	add_vertex(0, va);

	va.pos = a;
	va.color = color_blue;
	add_vertex(1, va);
	va.pos = up;
	add_vertex(1, va);
	add_vertex(0, va);
}

void draw_bbox(BBox bbox, Color c)
{
	Vec3 vertices[8];
	bbox_vertices(&bbox, vertices);

	Vertex va;
	va.color = c;

	for (int i = 0; i < 4; i++)
	{
		va.pos = vertices[i];
		add_vertex(1, va);
		va.pos = vertices[(i + 1) % 4];
		add_vertex(1, va);

		va.pos = vertices[i + 4];
		add_vertex(1, va);
		va.pos = vertices[(i + 1) % 4 + 4];
		add_vertex(1, va);

		va.pos = vertices[i];
		add_vertex(1, va);
		va.pos = vertices[i + 4];
		add_vertex(1, va);
	}
}

inline void fill_face(Vertex *va, const Vec3 *a, const Vec3 *b, const Vec3 *c, const Vec3 *d)
{

	va->pos = *a;
	add_vertex(2, *va);
	va->pos = *b;
	add_vertex(2, *va);
	va->pos = *c;
	add_vertex(2, *va);

	va->pos = *a;
	add_vertex(2, *va);
	va->pos = *c;
	add_vertex(2, *va);
	va->pos = *d;
	add_vertex(2, *va);
}
void fill_bbox(BBox bbox, Color c)
{
	Vec3 vertices[8];
	bbox_vertices(&bbox, vertices);

	Vertex va;
	va.color = c;

	fill_face(&va, &vertices[1], &vertices[5], &vertices[6], &vertices[2]);
	fill_face(&va, &vertices[0], &vertices[4], &vertices[7], &vertices[3]);

	fill_face(&va, &vertices[3], &vertices[7], &vertices[6], &vertices[2]);
	fill_face(&va, &vertices[0], &vertices[4], &vertices[5], &vertices[1]);

	fill_face(&va, &vertices[4], &vertices[5], &vertices[6], &vertices[7]);
	fill_face(&va, &vertices[0], &vertices[1], &vertices[2], &vertices[3]);
}

void draw_cube(Vec3 a, Color c, Vec3 s)
{
	Vec3 size = vec3_mulf(s, 0.5f);
	draw_bbox(bbox(vec3_sub(a, size), vec3_add(a, size)), c);
}

void draw_cubef(Vec3 a, Color c, float s)
{
	Vec3 size = vec3f(s * 0.5f);
	draw_bbox(bbox(vec3_sub(a, size), vec3_add(a, size)), c);
}

void draw_edge(Edge e, Color c)
{
	Vertex va;
	va.color = c;
	va.pos = e.a;
	add_vertex(1, va);
	va.pos = e.b;
	add_vertex(1, va);
}

void draw_triangle(Triangle t, Color c)
{
	Vertex va;
	va.color = c;
	va.pos = t.a;
	add_vertex(1, va);
	va.pos = t.b;
	add_vertex(1, va);

	va.pos = t.b;
	add_vertex(1, va);
	va.pos = t.c;
	add_vertex(1, va);

	va.pos = t.c;
	add_vertex(1, va);
	va.pos = t.a;
	add_vertex(1, va);
}
void draw_tetrahedron(Tetrahedron t, Color c)
{
	Vertex va;
	va.color = c;
	va.pos = t.a;
	add_vertex(1, va);
	va.pos = t.b;
	add_vertex(1, va);

	va.pos = t.b;
	add_vertex(1, va);
	va.pos = t.c;
	add_vertex(1, va);

	va.pos = t.c;
	add_vertex(1, va);
	va.pos = t.a;
	add_vertex(1, va);

	va.pos = t.a;
	add_vertex(1, va);
	va.pos = t.d;
	add_vertex(1, va);

	va.pos = t.b;
	add_vertex(1, va);
	va.pos = t.d;
	add_vertex(1, va);

	va.pos = t.c;
	add_vertex(1, va);
	va.pos = t.d;
	add_vertex(1, va);
}

void draw_circleXY(Vec3 a, Color c, float r, int s)
{
	float p = 360.0f / s;
	float sp = sind(p);
	float cp = cosd(p);
	Vec3 r1 = {1.0f, 0.0f, 0.0f};
	Vec3 v1 = vec3_mulf(r1, r);
	v1 = vec3_add(v1, a);
	for (int i = 0; i < s; i++)
	{
		Vec3 r2;
		r2.x = cp * r1.x - sp * r1.y;
		r2.y = sp * r1.x + cp * r1.y;
		r2.z = 0;
		Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);

		Vertex va;
		va.color = c;
		va.pos = v1;
		add_vertex(1, va);
		va.pos = v2;
		add_vertex(1, va);

		r1 = r2;
		v1 = v2;
	}
}
void draw_circleXZ(Vec3 a, Color c, float r, int s)
{
	float p = 360.0f / s;
	float sp = sind(p);
	float cp = cosd(p);
	Vec3 r1 = {1.0f, 0.0f, 0.0f};
	Vec3 v1 = vec3_mulf(r1, r);
	v1 = vec3_add(v1, a);
	for (int i = 0; i < s; i++)
	{
		Vec3 r2;
		r2.x = cp * r1.x - sp * r1.z;
		r2.z = sp * r1.x + cp * r1.z;
		r2.y = 0;
		Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);

		Vertex va;
		va.color = c;
		va.pos = v1;
		add_vertex(1, va);
		va.pos = v2;
		add_vertex(1, va);

		r1 = r2;
		v1 = v2;
	}
}

void draw_circleYZ(Vec3 a, Color c, float r, int s)
{
	float p = 360.0f / s;
	float sp = sind(p);
	float cp = cosd(p);
	Vec3 r1 = {0.0f, 1.0f, 0.0f};
	Vec3 v1 = vec3_mulf(r1, r);
	v1 = vec3_add(v1, a);
	for (int i = 0; i < s; i++)
	{
		Vec3 r2;
		r2.y = cp * r1.y - sp * r1.z;
		r2.z = sp * r1.y + cp * r1.z;
		r2.x = 0;
		Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);

		Vertex va;
		va.color = c;
		va.pos = v1;
		add_vertex(1, va);
		va.pos = v2;
		add_vertex(1, va);

		r1 = r2;
		v1 = v2;
	}
}

void draw_sphere(Vec3 a, Color c, float r, int s)
{
	draw_circleXZ(a, c, r, s);
	draw_circleXY(a, c, r, s);
	draw_circleYZ(a, c, r, s);
}
