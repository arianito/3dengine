#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

typedef struct
{
    GLint programId;
} Shader;

Shader *shader_create(const char *vs, const char *fs);
Shader *shader_load(const char *vs, const char *fs);
void shader_destroy(Shader *p);

inline void shader_begin(const Shader *p)
{
    glUseProgram(p->programId);
}

inline void shader_end()
{
    glUseProgram(0);
}

inline void shader_uint(const Shader *p, const char *name, unsigned int a)
{
    glUniform1ui(glGetUniformLocation(p->programId, name), a);
}

inline void shader_int(const Shader *p, const char *name, int a)
{
    glUniform1i(glGetUniformLocation(p->programId, name), a);
}

inline void shader_vec2(const Shader *p, const char *name, const void *v)
{
    glUniform2fv(glGetUniformLocation(p->programId, name), 1, v);
}
inline void shader_vec3(const Shader *p, const char *name, const void *v)
{
    glUniform3fv(glGetUniformLocation(p->programId, name), 1, v);
}

inline void shader_vec4(Shader *p, const char *name, const void *v)
{
    glUniform4fv(glGetUniformLocation(p->programId, name), 1, v);
}

inline void shader_mat4(const Shader *p, const char *name, const void *v)
{
    glUniformMatrix4fv(glGetUniformLocation(p->programId, name), 1, GL_FALSE, v);
}

inline void shader_float(const Shader *p, const char *name, float f)
{
    glUniform1f(glGetUniformLocation(p->programId, name), f);
}
