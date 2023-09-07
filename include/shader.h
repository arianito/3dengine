#pragma once
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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

typedef GLint Shader;

Shader shader_create(const char *vs, const char *fs);
Shader shader_load(const char *vs, const char *fs);
void shader_destroy(Shader p);

inline void shader_begin(Shader p)
{
    glUseProgram(p);
}

inline void shader_end()
{
    glUseProgram(0);
}

inline void shader_uint(Shader p, const char *name, unsigned int a)
{
    glUniform1ui(glGetUniformLocation(p, name), a);
}

inline void shader_int(Shader p, const char *name, int a)
{
    glUniform1i(glGetUniformLocation(p, name), a);
}

inline void shader_vec2(Shader p, const char *name, const void *v)
{
    glUniform2fv(glGetUniformLocation(p, name), 1, v);
}
inline void shader_vec3(Shader p, const char *name, const void *v)
{
    glUniform3fv(glGetUniformLocation(p, name), 1, v);
}

inline void shader_vec4(Shader p, const char *name, const void *v)
{
    glUniform4fv(glGetUniformLocation(p, name), 1, v);
}

inline void shader_mat4(Shader p, const char *name, const void *v)
{
    glUniformMatrix4fv(glGetUniformLocation(p, name), 1, GL_FALSE, v);
}

inline void shader_float(Shader p, const char *name, float f)
{
    glUniform1f(glGetUniformLocation(p, name), f);
}
