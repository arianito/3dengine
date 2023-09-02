#include "game.h"

#include "malloc.h"
#include "stdio.h"
#include "mathf.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static int frames = 0;
static float last_check = 0.0f;

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
    game = (Game *)malloc(sizeof(Game));
    game->delta_time = 1 / 60.0f;
    game->fps = 60;
    game->width = 800;
    game->height = 600;
    game->time = 0;
    game->ratio = game->width / game->height;

    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_SAMPLES, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    game->window = glfwCreateWindow((int)game->width, (int)game->height, "Game", NULL, NULL);
    if (!game->window)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(game->window);
    glfwSetFramebufferSizeCallback(game->window, framebuffer_size_callback);

    glfwSetInputMode(game->window, GLFW_STICKY_KEYS, GL_TRUE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return;

    glfwSwapInterval(1);
    glfwSetTime(0);

    GLint w, h;
    glfwGetFramebufferSize(game->window, &w, &h);
    framebuffer_size_callback(game->window, w, h);
}

inline void calculate_fps()
{
    game->delta_time = (float)glfwGetTime() - game->time;
    game->time = (float)glfwGetTime();
    frames++;
    float f = (int)(floorf(game->time));
    if (f != last_check)
    {
        game->fps = frames;
        frames = 0;
        last_check = f;
    }
}

char game_loop()
{
    glfwSwapBuffers(game->window);
    glfwPollEvents();
    calculate_fps();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_ADD);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    return !glfwWindowShouldClose(game->window);
}

void game_terminate()
{
    free(game);
    glfwTerminate();
}