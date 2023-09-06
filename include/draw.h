#pragma once

#include "mathf.h"


void draw_init();
void draw_render();
void draw_terminate();

void draw_point(Vec3 pos, Color c, float size);
void draw_line(Vec3 a, Vec3 b, Color c);
void draw_axis(Vec3 a, Quat q, float scale);
void draw_axisRot(Vec3 a, Rot r, float scale);

void draw_bbox(BBox bbox, Color c);
void fill_bbox(BBox bbox, Color c);
void draw_cube(Vec3 a, Color c, Vec3 s);
void draw_cubef(Vec3 a, Color c, float s);

void draw_edge(Edge e, Color c);
void draw_triangle(Triangle t, Color c);
void draw_tetrahedron(Tetrahedron t, Color c);

void draw_circleXY(Vec3 a, Color c, float r, int s);
void draw_circleXZ(Vec3 a, Color c, float r, int s);
void draw_circleYZ(Vec3 a, Color c, float r, int s);
void draw_sphere(Vec3 a, Color c, float r, int s);