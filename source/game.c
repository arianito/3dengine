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
#include "game.h"

#include <stdio.h>
#include "memory/alloc.h"
#include "mathf.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

Game *game = NULL;
Time *time = NULL;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
#if __APPLE__
	game->width = (float)width * 0.5f;
	game->height = (float)height * 0.5f;
#else
	game->width = (float)width;
	game->height = (float)height;
#endif
	game->ratio = game->width / game->height;
	glViewport(0, 0, width, height);
}

void game_init()
{
	time = alloc_global(Time);
	time->deltaTime = 1 / 60.0f;
	time->time = 0;

	game = alloc_global(Game);
	game->fps = 60;
	game->width = 1200;
	game->height = 780;
	game->ratio = game->width / game->height;
	game->fullScreen = 0;

	if (!glfwInit())
		return;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	game->screenWidth = (float)mode->width;
	game->screenHeight = (float)mode->height;

	if (game->fullScreen)
	{
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
		game->width = game->screenWidth;
		game->height = game->screenHeight;
	}

	game->window = glfwCreateWindow((int)game->width, (int)game->height, "Game", game->fullScreen ? monitor : NULL, NULL);
	if (!game->window)
	{
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(game->window);
	glfwSetFramebufferSizeCallback(game->window, framebuffer_size_callback);

	glfwSetInputMode(game->window, GLFW_CURSOR, GL_TRUE);
	glfwSetInputMode(game->window, GLFW_STICKY_KEYS, GL_TRUE);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return;

	glfwSwapInterval(1);
	glfwSetTime(0);

	GLint w, h;
	glfwGetFramebufferSize(game->window, &w, &h);
	framebuffer_size_callback(game->window, w, h);
}

static int frames = 0;
static int lastCheck = 0;

inline void calculate_fps()
{
	time->deltaTime = (float)glfwGetTime() - time->time;
	time->time = (float)glfwGetTime();
	frames++;
	int f = (int)(floorf(time->time));
	if (f != lastCheck)
	{
		game->fps = frames;
		frames = 0;
		lastCheck = f;
	}
}

char game_loop()
{
	glfwSwapBuffers(game->window);
	glfwPollEvents();
	calculate_fps();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_ADD);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	if (glfwGetKey(game->window, GLFW_KEY_ESCAPE))
		return 0;

	return !glfwWindowShouldClose(game->window);
}

void game_terminate()
{
	glfwTerminate();
}