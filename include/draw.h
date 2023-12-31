#pragma once


#include "mathf.h"


void draw_init();

void draw_render();

void draw_terminate();

void add_vertex(int type, Vertex v);

void draw_point(Vec3 pos, float size, Color c);

void draw_line(Vec3 a, Vec3 b, Color c);

void draw_bbox(BBox bbox, Color c);

void fill_bbox(BBox bbox, Color c);

void draw_cube(Vec3 a, Vec3 s, Color c);

void fill_cube(Vec3 a, Vec3 s, Color c);

void draw_cubef(Vec3 a, float s, Color c);

void fill_cubef(Vec3 a, float s, Color c);

void draw_edge(Edge e, Color c);

void draw_triangle(Triangle t, Color c);

void draw_tetrahedron(Tetrahedron t, Color c);

void fill_tetrahedron(Tetrahedron t, Color c);

void draw_circleXY(Vec3 a, float r, Color c, int s);

void draw_circleXZ(Vec3 a, float r, Color c, int s);

void draw_circleYZ(Vec3 a, float r, Color c, int s);
void fill_circleYZ(Vec3 a, float r, Color c, int s);

void draw_sphere(Vec3 a, float r, Color c, int s);

void draw_arrow(Vec3 a, Vec3 b, Vec3 up, Color c, float p);

void draw_ray(Ray r, Color c);

void draw_axis(Vec3 a, float scale, Quat q);

void draw_axisRot(Vec3 a, float scale, Rot r);

void draw_frustum(Vec3 pos, Rot rt, float fov, float ratio, float nr, float fr, Color c);