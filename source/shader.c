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