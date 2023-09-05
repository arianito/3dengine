#include "shader.h"

#include <stdio.h>
#include <assert.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "file.h"


Shader shader_create(const char *vs, const char *fs)
{
    GLint status = GL_TRUE;
    char error_msg[1024];
    GLsizei read;
    GLuint vsp = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsp, 1, &vs, NULL);
    glCompileShader(vsp);
    glGetShaderiv(vsp, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(vsp, 1024, &read, error_msg);
        printf("vs error: %s", error_msg);
        return 0;
    }
    GLuint fsp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsp, 1, &fs, NULL);
    glCompileShader(fsp);
    glGetShaderiv(fsp, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(fsp, 1024, &read, error_msg);
        printf("fs error: %s", error_msg);
        return 0;
    }
    GLint programId = glCreateProgram();
    glAttachShader(programId, vsp);
    glAttachShader(programId, fsp);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetProgramInfoLog(programId, 1024, &read, error_msg);
        printf("compile error: %s", error_msg);
        return 0;
    }
    glDetachShader(programId, vsp);
    glDetachShader(programId, fsp);
    glDeleteShader(vsp);
    glDeleteShader(fsp);

    return programId;
}

Shader shader_load(const char *vs, const char *fs)
{
    File *vsf = file_read(vs);
    File *fsf = file_read(fs);

    Shader sh = shader_create(vsf->text, fsf->text);

    file_destroy(fsf);
    file_destroy(vsf);
    return sh;
}

void shader_destroy(Shader p)
{
    glDeleteProgram(p);
}