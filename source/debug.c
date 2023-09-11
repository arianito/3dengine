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
#include "camera.h"
#include "file.h"
#include "draw.h"

#include <stdarg.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define BUFFER_OFFSET(x) ((const void *)(x))

enum
{
	max_space = 100 * KILOBYTES,
	max_elements = 1000,
};

typedef struct
{
	Transform transform;
	Vec2 origin;
	Color color;
	const char *text;

} Text3DData;

typedef struct
{
	Vec3 position;
	Vec2 origin;
	Color color;
	const char *text;

} Text2DData;

typedef struct
{
	Shader shader;
	GLuint vaoIds[2];
	GLuint vboIds[2];
	GLuint eboIds[2];

	Text2DData data2d[max_elements];
	Text3DData data3d[max_elements];

	int count2d;
	int count3d;

	GLuint fontTexture[1];

	Vec2 bound2d;
	Vec2 bound3d;
	Vec2 origin;
	Color color;

	char enabled;

	ArenaMemory *arena;

} DrawData;

static DrawData *debugData;

typedef struct
{
	Vec3 pos;
	Vec2 coord;
} Quad;

void debug_init()
{
	debugData = alloc_global(DrawData, sizeof(DrawData));
	clear(debugData, sizeof(DrawData));

	debugData->arena = arena_create(alloc_global(void, max_space), max_space);

	debugData->enabled = 1;
	debugData->origin = vec2_zero;
	debugData->color = color_white;
	debugData->shader = shader_load("shaders/debug.vs", "shaders/debug.fs");

	// vao
	glGenVertexArrays(2, debugData->vaoIds);
	glGenBuffers(2, debugData->vboIds);
	glGenBuffers(2, debugData->eboIds);

	{
		float w = 8.0f;
		float h = 24.0f;
		debugData->bound2d = vec2(w, h);
		float offset = (h - w) / (h * 2.0f);
		Quad vertices[] = {
			{vec3(0, 0, 0), vec2(offset, 0)},
			{vec3(w, 0, 0), vec2(1 - offset, 0)},
			{vec3(w, h, 0), vec2(1 - offset, 1)},
			{vec3(0, h, 0), vec2(offset, 1)},
		};
		unsigned int indices[] = {0, 1, 3, 1, 2, 3};

		glBindVertexArray(debugData->vaoIds[1]);
		glBindBuffer(GL_ARRAY_BUFFER, debugData->vboIds[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugData->eboIds[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(16));
	}

	{
		float w = 2.5f;
		float h = 8.0f;
		debugData->bound3d = vec2(w, h);
		float offset = (h - w) / (h * 2.0f);
		Quad vertices[] = {
			{vec3(0, 0, 0), vec2(offset, 0)},
			{vec3(0, -w, 0), vec2(1 - offset, 0)},
			{vec3(0, -w, -h), vec2(1 - offset, 1)},
			{vec3(0, 0, -h), vec2(offset, 1)},
		};
		unsigned int indices[] = {0, 1, 3, 1, 2, 3};

		glBindVertexArray(debugData->vaoIds[0]);
		glBindBuffer(GL_ARRAY_BUFFER, debugData->vboIds[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugData->eboIds[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(16));
	}

	// texture
	int width, height, nrChannels;
	char *path = resolve("fonts/consolas.png");
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	alloc_free(path);
	if (data == NULL)
	{
		printf("debug: failed to load font \n");
		debugData->enabled = 0;
		return;
	}

	glGenTextures(1, debugData->fontTexture);
	glBindTexture(GL_TEXTURE_2D, debugData->fontTexture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

Vec2 calculateSpace(const char *str, Vec2 bound)
{
	Vec2 v = {0, 0};
	Vec3 of = {0, 0, 0};
	int i = 0;
	while (*str != '\0')
	{
		if (*str == '\n')
		{
			of.x = 0;
			of.y += bound.y;
			str++;
		}
		of.x += bound.x;
		str++;
		i++;

		if (of.x > v.x)
			v.x = of.x;
		if (of.y > v.y)
			v.y = of.y;
	}
	if (i > 1)
		v.y += bound.y;
	return v;
}

void debug_render()
{
	if (!debugData->enabled)
		return;

	if (debugData->count2d == 0 && debugData->count3d == 0)
		return;

	shader_begin(debugData->shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, debugData->fontTexture[0]);

	if (debugData->count2d > 0)
	{
		Mat4 ortho = mat4_orthographic(0, game->width, game->height, 0, -1.0f, 1.0f);

		shader_mat4(debugData->shader, "projection", &ortho);
		shader_mat4(debugData->shader, "view", &mat4_identity);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		for (int i = 0; i < debugData->count2d; i++)
		{
			Text2DData it = debugData->data2d[i];
			Mat4 ma = mat4_mul(mat4_scalef(it.position.z), mat4_origin(vec3(it.position.x, it.position.y, 0)));
			shader_mat4(debugData->shader, "world", &ma);
			shader_vec4(debugData->shader, "color", &it.color);


			Vec2 space = calculateSpace(it.text, debugData->bound2d);
			space.x *= it.origin.x;
			space.y *= it.origin.y;
			Vec3 of = {-space.x, -space.y, 0};
			while (*it.text != '\0')
			{
				if (*it.text == '\n')
				{
					of.x = -space.x;
					of.y += debugData->bound2d.y;
					it.text++;
				}
				shader_vec3(debugData->shader, "offset", &of);
				shader_int(debugData->shader, "character", (int)(*it.text));

				glBindVertexArray(debugData->vaoIds[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				of.x += debugData->bound2d.x;
				it.text++;
			}
		}
	}

	if (debugData->count3d > 0)
	{
		shader_mat4(debugData->shader, "projection", &camera->projection);
		shader_mat4(debugData->shader, "view", &camera->view);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		for (int i = 0; i < debugData->count3d; i++)
		{
			Text3DData it = debugData->data3d[i];

			Mat4 m = mat4_transform(it.transform);
			shader_mat4(debugData->shader, "world", &m);
			shader_vec4(debugData->shader, "color", &it.color);

			Vec2 space = calculateSpace(it.text, debugData->bound3d);
			space.x *= it.origin.x;
			space.y *= it.origin.y;
			Vec3 of = {0, space.x, space.y};
			while (*it.text != '\0')
			{
				if (*it.text == '\n')
				{
					of.y = space.x;
					of.z -= debugData->bound3d.y;
					it.text++;
				}
				shader_vec3(debugData->shader, "offset", &of);
				shader_int(debugData->shader, "character", (int)(*it.text));

				glBindVertexArray(debugData->vaoIds[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				of.y -= debugData->bound3d.x;
				it.text++;
			}
		}
	}

	shader_end();

	debugData->count2d = 0;
	debugData->count3d = 0;
	arena_reset(debugData->arena);
}

void debug_terminate()
{
	glDeleteVertexArrays(2, debugData->vaoIds);
	glDeleteBuffers(2, debugData->vboIds);
	glDeleteBuffers(2, debugData->eboIds);
	glDeleteTextures(1, debugData->fontTexture);
	shader_destroy(debugData->shader);
}

void debug_origin(Vec2 origin)
{
	debugData->origin = origin;
}

void debug_color(Color color)
{
	debugData->color = color;
}

void debug_string(Vec3 pos, const char *str, int n)
{
	if (debugData->count2d == max_elements)
		debugData->count2d = 0;

	char *cpy = arena_alloc(debugData->arena, n, sizeof(size_t));
	memcpy(cpy, str, n);
	Text2DData dt;
	dt.position = pos;
	dt.text = cpy;
	dt.origin = debugData->origin;
	dt.color = debugData->color;

	debugData->data2d[debugData->count2d++] = dt;
}

void debug_string3d(Transform t, const char *str, int n)
{
	if (debugData->count3d == max_elements)
		debugData->count3d = 0;

	char *cpy = arena_alloc(debugData->arena, n, sizeof(size_t));
	memcpy(cpy, str, n);
	Text3DData dt;
	dt.transform = t;
	dt.text = cpy;
	dt.origin = debugData->origin;
	dt.color = debugData->color;

	debugData->data3d[debugData->count3d++] = dt;
}

void debug_stringf(Vec3 pos, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(NULL, 0, fmt, args);
	char *buffer = alloc_stack(char, len + 1);

	if (buffer != NULL)
	{
		vsnprintf(buffer, len + 1, fmt, args);
		debug_string(pos, buffer, len + 1);
	}

	alloc_free(buffer);
	va_end(args);
}

void debug_string3df(Transform t, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(NULL, 0, fmt, args);
	char *buffer = alloc_stack(char, len + 1);

	if (buffer != NULL)
	{
		vsnprintf(buffer, len + 1, fmt, args);
		debug_string3d(t, buffer, len + 1);
	}

	alloc_free(buffer);
	va_end(args);
}