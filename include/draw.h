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

#include "mathf.h"


void draw_init();

void draw_render();

void draw_terminate();

void draw_point(Vec3 pos, Color c, float size);

void draw_line(Vec3 a, Vec3 b, Color c);

void draw_bbox(BBox bbox, Color c);

void fill_bbox(BBox bbox, Color c);

void draw_cube(Vec3 a, Vec3 s, Color c);

void draw_cubef(Vec3 a, float s, Color c);

void draw_edge(Edge e, Color c);

void draw_triangle(Triangle t, Color c);

void draw_tetrahedron(Tetrahedron t, Color c);

void fill_tetrahedron(Tetrahedron t, Color c);

void draw_circleXY(Vec3 a, float r, Color c, int s);

void draw_circleXZ(Vec3 a, float r, Color c, int s);

void draw_circleYZ(Vec3 a, float r, Color c, int s);

void draw_sphere(Vec3 a, float r, Color c, int s);

void draw_arrow(Vec3 a, Vec3 b, Vec3 up, Color c, float p);

void draw_ray(Ray r, Color c);

void draw_axis(Vec3 a, Quat q, float scale);

void draw_axisRot(Vec3 a, Rot r, float scale);