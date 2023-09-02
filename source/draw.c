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

static Shader *shader;

enum
{
    types_n = 3,
    vertices_count = 1 * 4096,
    edges_count = 2 * vertices_count,
    triangles_count = 3 * vertices_count,
};

static GLuint m_vaoIds[types_n];
static GLuint m_vboIds[types_n];

static int types[types_n] = {GL_POINTS, GL_LINES, GL_TRIANGLES};
static int sizes[types_n] = {vertices_count, edges_count, triangles_count};
static int counter[types_n];

static Vertex vertices[types_n][vertices_count];

void draw_init()
{
    shader = shader_load("draw.vs", "draw.fs");
    glGenVertexArrays(types_n, m_vaoIds);
    glGenBuffers(types_n, m_vboIds);

    for (int i = 0; i < types_n; i++)
    {
        glBindVertexArray(m_vaoIds[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(16));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(32));

        counter[i] = 0;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_render()
{
    shader_begin(shader);
    shader_mat4(shader, "projection", &camera->projection);
    shader_mat4(shader, "view", &camera->view);

    glLineWidth(1);

    for (int i = 0; i < types_n; i++)
    {
        int count = counter[i];
        if (count == 0)
            continue;


        glBindVertexArray(m_vaoIds[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Vertex), vertices[i]);
        glDrawArrays(types[i], 0, count);

        counter[i] = 0;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_end();
}

void draw_terminate()
{
    glDeleteVertexArrays(types_n, m_vaoIds);
    glDeleteBuffers(types_n, m_vboIds);
    shader_destroy(shader);
}

void add_vertex(int type, Vertex v)
{
    int count = counter[type];
    if (counter[type] == sizes[type])
        draw_render();

    vertices[type][count] = v;
    counter[type]++;
}

void draw_vertex(Vertex v)
{
    add_vertex(0, v);
}

void draw_segment(Vertex v1, Vertex v2)
{
    add_vertex(1, v1);
    add_vertex(1, v2);
}
void draw_triangle(Vertex v1, Vertex v2, Vertex v3)
{
    add_vertex(2, v1);
    add_vertex(2, v2);
    add_vertex(2, v3);
}

void draw_point(Vec3 pos, Color c, float size)
{
    Vertex v;
    v.color = c;
    v.pos = pos;
    v.size = size;
    add_vertex(0, v);
}

void draw_edge(Edge e, Color c)
{
    Vertex v1;
    v1.pos = e.a;
    v1.color = c;
    Vertex v2;
    v2.pos = e.a;
    v2.color = c;
    draw_segment(v1, v2);
}

void draw_line(Vec3 a, Vec3 b, Color c)
{
    Vertex v1;
    v1.pos = a;
    v1.color = c;
    Vertex v2;
    v2.pos = b;
    v2.color = c;
    draw_segment(v1, v2);
}

void draw_circleXY(Vec3 a, Color c, float r, int s)
{
    float p = 360.0f / s;
    float sp = sind(p);
    float cp = cosd(p);
    Vec3 r1 = {1.0f, 0.0f, 0.0f};
    Vec3 v1 = vec3_mulf(r1, r);
    v1 = vec3_add(v1, a);
    for (int i = 0; i < s; ++i)
    {
        Vec3 r2;
        r2.x = cp * r1.x - sp * r1.y;
        r2.y = sp * r1.x + cp * r1.y;
        r2.z = 0;
        Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);
        draw_line(v1, v2, c);
        draw_point(v2, c, VERTEX_SIZE);
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
    for (int i = 0; i < s; ++i)
    {
        Vec3 r2;
        r2.x = cp * r1.x - sp * r1.z;
        r2.z = sp * r1.x + cp * r1.z;
        r2.y = 0;
        Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);
        draw_line(v1, v2, c);
        draw_point(v2, c, VERTEX_SIZE);
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
    for (int i = 0; i < s; ++i)
    {
        Vec3 r2;
        r2.y = cp * r1.y - sp * r1.z;
        r2.z = sp * r1.y + cp * r1.z;
        r2.x = 0;
        Vec3 v2 = vec3_add(vec3_mulf(r2, r), a);
        draw_line(v1, v2, c);
        draw_point(v2, c, VERTEX_SIZE);
        r1 = r2;
        v1 = v2;
    }
}

void draw_axis(Vec3 a, Quat q, float s)
{
    Vec3 forward = vec3_add(a, vec3_mulf(quat_forward(q), s));
    Vec3 right = vec3_add(a, vec3_mulf(quat_right(q), s));
    Vec3 up = vec3_add(a, vec3_mulf(quat_up(q), s));

    draw_line(a, forward, color_red);
    draw_line(a, right, color_green);
    draw_line(a, up, color_blue);

    draw_point(forward, color_red, VERTEX_SIZE);
    draw_point(right, color_green, VERTEX_SIZE);
    draw_point(up, color_blue, VERTEX_SIZE);
    draw_point(a, color_yellow, VERTEX_SIZE);
}

void draw_axisRot(Vec3 a, Rot r, float s)
{
    Vec3 forward = vec3_add(a, vec3_mulf(rot_forward(r), s));
    Vec3 right = vec3_add(a, vec3_mulf(rot_right(r), s));
    Vec3 up = vec3_add(a, vec3_mulf(rot_up(r), s));

    draw_line(a, forward, color_red);
    draw_line(a, right, color_green);
    draw_line(a, up, color_blue);

    draw_point(forward, color_red, VERTEX_SIZE);
    draw_point(right, color_green, VERTEX_SIZE);
    draw_point(up, color_blue, VERTEX_SIZE);
    draw_point(a, color_yellow, VERTEX_SIZE);
}

void draw_bbox(BBox bbox, Color c)
{
    Vec3 vertices[8];
    bbox_vertices(&bbox, vertices);

    for (int i = 0; i < 8; i++)
        draw_point(vertices[i], c, VERTEX_SIZE);

    draw_line(vertices[0], vertices[1], c);
    draw_line(vertices[1], vertices[2], c);
    draw_line(vertices[2], vertices[3], c);
    draw_line(vertices[3], vertices[0], c);

    draw_line(vertices[4], vertices[5], c);
    draw_line(vertices[5], vertices[6], c);
    draw_line(vertices[6], vertices[7], c);
    draw_line(vertices[7], vertices[4], c);

    draw_line(vertices[0], vertices[4], c);
    draw_line(vertices[1], vertices[5], c);
    draw_line(vertices[2], vertices[6], c);
    draw_line(vertices[3], vertices[7], c);
}

void draw_sphere(Vec3 a, Color c, float r, int s)
{
    draw_circleXZ(a, c, r, s);
    draw_circleXY(a, c, r, s);
    draw_circleYZ(a, c, r, s);
}
void draw_cube(Vec3 a, Color c, float s)
{
    Vec3 size = vec3f(s * 0.5f);
    draw_bbox(bbox(vec3_sub(a, size), vec3_add(a, size)), c);
}