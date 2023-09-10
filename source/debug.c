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

#include <stdarg.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define BUFFER_OFFSET(x) ((const void *)(x))

enum
{
	max_space = 2 * MEGABYTES,
	max_elements = 100,
};

typedef struct
{
	Transform transform;
	const char *text;

} Text3DData;

typedef struct
{
	Vec3 position;
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

	char enabled;

} DrawData;

static DrawData *drawData;

typedef struct
{
	Vec3 pos;
	Vec2 coord;
} Quad;

void debug_init()
{
	drawData = alloc_global(DrawData, sizeof(DrawData));
	clear(drawData, sizeof(DrawData));

	drawData->enabled = 1;
	drawData->shader = shader_load("shaders/debug.vs", "shaders/debug.fs");

	// vao
	glGenVertexArrays(2, drawData->vaoIds);
	glGenBuffers(2, drawData->vboIds);
	glGenBuffers(2, drawData->eboIds);

	{
		float w = 8.0f;
		float h = 24.0f;
		drawData->bound2d = vec2(w, h);
		float offset = (h - w) / (h * 2.0f);
		float offset2 = 2.0f / h;
		Quad vertices[] = {
			{vec3(0, 0, 0), vec2(offset, offset2)},
			{vec3(w, 0, 0), vec2(1 - offset, offset2)},
			{vec3(w, h, 0), vec2(1 - offset, 1 - offset2)},
			{vec3(0, h, 0), vec2(offset, 1 - offset2)},
		};
		unsigned int indices[] = {0, 1, 3, 1, 2, 3};

		glBindVertexArray(drawData->vaoIds[1]);
		glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawData->eboIds[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(16));
	}

	{
		float w = 2.5f;
		float h = 8.0f;
		drawData->bound3d = vec2(w, h);
		float offset = (h - w) / (h * 2.0f);
		float offset2 = 0.5f / h;
		Quad vertices[] = {
			{vec3(0, 0, 0), vec2(offset, offset2)},
			{vec3(0, -w, 0), vec2(1 - offset, offset2)},
			{vec3(0, -w, -h), vec2(1 - offset, 1 - offset2)},
			{vec3(0, 0, -h), vec2(offset, 1 - offset2)},
		};
		unsigned int indices[] = {0, 1, 3, 1, 2, 3};

		glBindVertexArray(drawData->vaoIds[0]);
		glBindBuffer(GL_ARRAY_BUFFER, drawData->vboIds[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawData->eboIds[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(16));
	}

	// texture
	int width, height, nrChannels;
	char o[URL_LENGTH];
	resolve("fonts/consolas.png", o);
	unsigned char *data = stbi_load(o, &width, &height, &nrChannels, 0);
	if (data == NULL)
	{
		printf("debug: failed to load font \n");
		drawData->enabled = 0;
		return;
	}

	glGenTextures(1, drawData->fontTexture);
	glBindTexture(GL_TEXTURE_2D, drawData->fontTexture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void debug_render()
{
	if (!drawData->enabled)
		return;

	if (drawData->count2d == 0 && drawData->count3d == 0)
		return;

	glDisable(GL_DEPTH_TEST);
	shader_begin(drawData->shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, drawData->fontTexture[0]);

	if (drawData->count2d > 0)
	{
		Mat4 ortho = mat4_orthographic(0, game->width, game->height, 0, -1.0f, 1.0f);

		shader_mat4(drawData->shader, "projection", &ortho);
		shader_mat4(drawData->shader, "view", &mat4_identity);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		for (int i = 0; i < drawData->count2d; i++)
		{
			Text2DData it = drawData->data2d[i];

			Mat4 ma = mat4_mul(mat4_scalef(it.position.z), mat4_origin(vec3(it.position.x, it.position.y, 0)));
			shader_mat4(drawData->shader, "world", &ma);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, drawData->fontTexture[0]);

			Vec3 of = {0, 0, 0};
			while (*it.text != '\0')
			{
				if (*it.text == '\n')
				{
					of.x = 0;
					of.y += drawData->bound2d.y;
					it.text++;
				}
				shader_vec3(drawData->shader, "offset", &of);
				shader_int(drawData->shader, "character", (int)(*it.text));

				glBindVertexArray(drawData->vaoIds[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				of.x += drawData->bound2d.x;
				it.text++;
			}
		}
	}

	if (drawData->count3d > 0)
	{
		shader_mat4(drawData->shader, "projection", &camera->projection);
		shader_mat4(drawData->shader, "view", &camera->view);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		for (int i = 0; i < drawData->count3d; i++)
		{
			Text3DData it = drawData->data3d[i];

			Mat4 m = mat4_transform(it.transform);
			shader_mat4(drawData->shader, "world", &m);

			Vec3 of = {0, 0, 0};
			while (*it.text != '\0')
			{
				if (*it.text == '\n')
				{
					of.y = 0;
					of.z -= drawData->bound3d.y;
					it.text++;
				}
				shader_vec3(drawData->shader, "offset", &of);
				shader_int(drawData->shader, "character", (int)(*it.text));

				glBindVertexArray(drawData->vaoIds[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				of.y -= drawData->bound3d.x;
				it.text++;
			}
		}
	}

	shader_end();
	glEnable(GL_DEPTH_TEST);

	drawData->count2d = 0;
	drawData->count3d = 0;
}

void debug_terminate()
{
	glDeleteVertexArrays(2, drawData->vaoIds);
	glDeleteBuffers(2, drawData->vboIds);
	glDeleteBuffers(2, drawData->eboIds);
	glDeleteTextures(1, drawData->fontTexture);
	shader_destroy(drawData->shader);
}

void debug_string(Vec3 pos, const char *str, int n)
{
	Text2DData dt;
	dt.position = pos;
	dt.text = str;

	drawData->data2d[drawData->count2d++] = dt;
}

void debug_string3d(Transform t, const char *str, int n)
{
	Text3DData dt;
	dt.transform = t;
	dt.text = str;
	drawData->data3d[drawData->count3d++] = dt;
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