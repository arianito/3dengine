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

#include <immintrin.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_AVX_256
// #define USE_AVX_128

typedef struct
{
	float x;
	float y;
} Vec2;

typedef struct
{
	float x;
	float y;
	float z;
	char __padding[4];
} Vec3;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} Vec4;

typedef struct
{
	float pitch;
	float yaw;
	float roll;
	char __padding[4];
} Rot;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} Quat;

typedef struct
{
	Quat rotation;
	Vec3 position;
} Transform;

typedef struct
{
	Vec3 a;
	Vec3 b;
	Vec3 c;
} Triangle;

typedef struct
{
	Vec3 a;
	Vec3 b;
} Edge;

typedef struct
{
	Vec3 a;
	Vec3 b;
	Vec3 c;
	Vec3 d;
} Tetrahedron;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} Plane;

typedef struct
{
	float r;
	float g;
	float b;
	float a;
} Color;

typedef struct
{
	Vec3 pos;
	Color color;
	float size;
	char __padding0[4];
} Vertex;

typedef struct
{
	Vec3 a;
	Vec3 b;
} BBox;

typedef struct
{
	float m[4][4];
} Mat4;

typedef enum
{
	UNIT_AXIS_X,
	UNIT_AXIS_Y,
	UNIT_AXIS_Z,
} UnitAxisEnum;

static const float RAD2DEG = 57.295779513082321f;
static const float DEG2RAD = 0.017453292519943f;
static const float PI = 3.141592653589793f;
static const float HALF_PI = 1.570796326794896f;
static const float EPSILON = 0.000001f;
static const float EPSILON2 = 0.0000000001f;
static const float MIN = 1.17549e-38f;
static const float MAX = 3.40282e+38f;

static const Vec2 vec2_zero = {0, 0};
static const Vec2 vec2_one = {1, 1};
static const Vec2 vec2_up = {0, 1};
static const Vec2 vec2_down = {0, -1};
static const Vec2 vec2_left = {-1, 0};
static const Vec2 vec2_right = {1, 0};

static const Vec3 vec3_zero = {0, 0, 0};
static const Vec3 vec3_one = {1, 1, 1};
static const Vec3 vec3_forward = {1, 0, 0};
static const Vec3 vec3_backward = {-1, 0, 0};
static const Vec3 vec3_right = {0, 1, 0};
static const Vec3 vec3_left = {0, -1, 0};
static const Vec3 vec3_up = {0, 0, 1};
static const Vec3 vec3_down = {0, 0, -1};

static const Vec4 vec4_zero = {0, 0, 0, 0};
static const Vec4 vec4_one = {1, 1, 1, 1};

static const Color color_black = {0, 0, 0, 1};
static const Color color_white = {1, 1, 1, 1};
static const Color color_red = {1, 0, 0, 1};
static const Color color_darkred = {0.5, 0, 0, 1};
static const Color color_green = {0, 1, 0, 1};
static const Color color_blue = {0, 0, 1, 1};
static const Color color_yellow = {1, 1, 0, 1};
static const Color color_gray = {0.5, 0.5, 0.5, 1};

static const Rot rot_zero = {0, 0, 0};
static const Quat quat_identity = {0, 0, 0, 1};
static const Mat4 mat4_identity = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};

// math
inline void seedf(unsigned int seed)
{
	srand(seed);
}

#define IMAX_BITS(m) ((m) / ((m) % 255 + 1) / 255 % 255 * 8 + 7 - 86 / ((m) % 255 + 12))
#define RAND_MAX_WIDTH IMAX_BITS(RAND_MAX)

inline float randf()
{
	unsigned int r = 0;
	for (int i = 0; i < 32; i += RAND_MAX_WIDTH)
	{
		r <<= RAND_MAX_WIDTH;
		r ^= (unsigned int)rand();
	}
	return (float)r / (float)0xffffffff;
}

inline float sind(float d) { return sinf(d * DEG2RAD); }
inline float cosd(float d) { return cosf(d * DEG2RAD); }
inline float tand(float d) { return tanf(d * DEG2RAD); }
inline float ramp(float a) { return a < 0 ? 0 : a; }
inline float asind(float a) { return asinf(a) * RAD2DEG; }
inline float square(float a) { return a * a; }
inline float acosd(float a) { return acosf(a) * RAD2DEG; }
inline float atand(float a) { return atanf(a) * RAD2DEG; }
inline float atan2d(float a, float b) { return atan2f(a, b) * RAD2DEG; }
inline float sign(float a) { return a >= 0 ? 1.0f : -1.0f; }
inline float selectf(float a, float b, float c) { return a >= 0 ? b : c; }
inline float invSqrt(float a) { return 1.0f / sqrtf(a); }

inline float clamp(float a, float min, float max)
{
	float z = a;
	if (a < min)
		z = min;
	else if (a > max)
		z = max;
	return z;
}

inline float clamp01(float a)
{
	float z = a;
	if (a < 0.0f)
		z = 0.0f;
	else if (a > 1.0f)
		z = 1.0f;
	return z;
}

inline float lerp01(float a, float b, float t) { return a + (b - a) * clamp01(t); }
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline char nearEq(float a, float b) { return fabsf(b - a) <= EPSILON; }
inline char near0(float a) { return nearEq(a, 0); }
inline float repeat(float t, float length) { return clamp(t - floorf(t / length) * length, 0.0f, length); }
inline float pingPong(float t, float length) { return length - fabsf(repeat(t, length * 2.0f) - length); }
inline float invLerp(float a, float b, float value) { return a != b ? clamp01((value - a) / (b - a)) : 0.0f; }

inline float slerp(float from, float to, float t)
{
	float t0 = clamp01(t);
	t0 = -2.0f * t0 * t0 * t0 + 3.0f * t0 * t0;
	return to * t0 + from * (1.0f - t0);
}

inline float deltaAngle(float current, float target)
{
	float delta = repeat((target - current), 360.0f);
	return delta > 180.0f ? delta - 360.0f : delta;
}

inline float lerpAngle(float a, float b, float t)
{
	float delta = repeat((b - a), 360.0f);
	if (delta > 180)
		delta -= 360.0f;
	return a + delta * clamp01(t);
}

inline float unwind(float a)
{
	while (a > 180.f)
		a -= 360.f;
	while (a < -180.f)
		a += 360.f;
	return a;
}

inline float clampAxis(float a)
{
	a = fmodf(a, 360.0f);
	if (a < 0.0f)
		a += 360.0f;

	return a;
}

inline float normAxis(float a)
{
	a = clampAxis(a);
	if (a > 180.0f)
		a -= 360.0f;

	return a;
}
inline float clampAngle(float a, float min, float max)
{

	float md = clampAxis(max - min) * 0.5f;
	float rc = clampAxis(min + md);
	float dc = clampAxis(a - rc);

	if (dc > md)
		return normAxis(rc + md);
	else if (dc < -md)
		return normAxis(rc - md);

	return normAxis(a);
}

inline char finite(float a)
{
	return _finite(a);
}

inline float snap(float a, float size)
{
	return floorf((a + size / 2.0f) / size) * size;
}

inline float moveTowards(float current, float target, float maxDelta)
{
	if (fabsf(target - current) <= maxDelta)
		return target;
	return current + sign(target - current) * maxDelta;
}

inline float moveTowardsAngle(float current, float target, float maxDelta)
{
	float da = deltaAngle(current, target);
	if (-maxDelta < da && da < maxDelta)
		return target;
	return moveTowards(current, current + da, maxDelta);
}

inline float smoothStep(float from, float to, float t)
{
	float t0 = clamp01(t);
	t0 = -2.0F * t0 * t0 * t0 + 3.0F * t0 * t0;
	return to * t0 + from * (1.0F - t0);
}

inline float smoothDamp(float current, float target, float *currentVelocity, float smoothTime,
						float maxSpeed, float deltaTime)
{
	float smoothTime0 = fmaxf(0.0001F, smoothTime);
	float omega = 2.0F / smoothTime0;
	float x = omega * deltaTime;
	float exp = 1.0F / (1.0F + x + 0.48F * x * x + 0.235F * x * x * x);
	float change = current - target;
	float originalTo = target;

	float maxChange = maxSpeed * smoothTime0;
	change = clamp(change, -maxChange, maxChange);
	float target0 = current - change;
	float temp = (*currentVelocity + omega * change) * deltaTime;
	*currentVelocity = (*currentVelocity - omega * temp) * exp;
	float output = target0 + (change + temp) * exp;

	if (originalTo - current > 0.0F == output > originalTo)
	{
		output = originalTo;
		*currentVelocity = (output - originalTo) / deltaTime;
	}

	return output;
}

// color

inline Color color(float r, float g, float b, float a)
{
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

inline Color color_alpha(Color c, float a)
{
	c.a = a;
	return c;
}

// vec2

inline Vec2 vec2(float x, float y)
{
	Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

inline Vec2 vec2f(float a)
{
	Vec2 v;
	v.x = a;
	v.y = a;
	return v;
}

inline Vec2 vec2_neg(Vec2 a)
{
	a.x *= -1.0f;
	a.y *= -1.0f;
	return a;
}

inline Vec2 vec2_add(Vec2 a, Vec2 b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline Vec2 vec2_addf(Vec2 a, float b)
{
	a.x += b;
	a.y += b;
	return a;
}

inline Vec2 vec2_sub(Vec2 a, Vec2 b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline Vec2 vec2_subf(Vec2 a, float b)
{
	a.x -= b;
	a.y -= b;
	return a;
}

inline Vec2 vec2_mulf(Vec2 a, float b)
{
	a.x *= b;
	a.y *= b;
	return a;
}

inline float vec2_dot(Vec2 a, Vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

inline float vec2_sqrMag(Vec2 a)
{
	return vec2_dot(a, a);
}

inline float vec2_mag(Vec2 a)
{
	return sqrtf(vec2_dot(a, a));
}

inline float vec2_dist(Vec2 a, Vec2 b)
{
	return vec2_mag(vec2_sub(a, b));
}

inline char vec2_eq(Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

inline char vec2_eq0(Vec2 a)
{
	return a.x == 0 && a.y == 0;
}

inline char vec2_nearEq(Vec2 a, Vec2 b)
{
	return vec2_dist(a, b) < EPSILON;
}

inline char vec2_near0(Vec2 a)
{
	return vec2_nearEq(a, vec2_zero);
}

inline Vec2 vec2_min(Vec2 a, Vec2 b)
{
	a.x = fminf(a.x, b.x);
	a.y = fminf(a.y, b.y);
	return a;
}

inline float vec2_minc(Vec2 a)
{
	return fminf(a.x, a.y);
}

inline Vec2 vec2_max(Vec2 a, Vec2 b)
{
	a.x = fmaxf(a.x, b.x);
	a.y = fmaxf(a.y, b.y);
	return a;
}

inline float vec2_maxc(Vec2 a)
{
	return fmaxf(a.x, a.y);
}

inline Vec2 vec2_abs(Vec2 a)
{
	a.x = fabsf(a.x);
	a.y = fabsf(a.y);
	return a;
}

inline float vec2_absMin(Vec2 a)
{
	return fminf(fabsf(a.x), fabsf(a.y));
}

inline float vec2_absMax(Vec2 a)
{
	return fmaxf(fabsf(a.x), fabsf(a.y));
}

inline Vec2 vec2_perpCw(Vec2 a)
{
	float ax = a.x;
	a.x = a.y;
	a.y = -ax;
	return a;
}

inline Vec2 vec2_perpCcw(Vec2 a)
{

	float ax = a.x;
	a.x = -a.y;
	a.y = ax;
	return a;
}

inline Vec2 vec2_norm(Vec2 a)
{
	float b = vec2_sqrMag(a);
	if (b == 1.0f)
		return a;
	if (b <= EPSILON2)
		return vec2_zero;

	float si = invSqrt(b);
	a.x *= si;
	a.y *= si;
	return a;
}

inline Vec2 vec2_sign(Vec2 a)
{
	a.x = sign(a.x);
	a.y = sign(a.y);
	return a;
}

inline Vec2 vec2_lerp(Vec2 a, Vec2 b, float dt)
{
	a.x = a.x + (b.x - a.x) * dt;
	a.y = a.y + (b.y - a.y) * dt;
	return a;
}

inline Vec2 vec2_lerp01(Vec2 a, Vec2 b, float dt)
{
	return vec2_lerp(a, b, clamp01(dt));
}

inline Vec2 vec2_reflect(Vec2 a, Vec2 n)
{
	return vec2_add(a, vec2_mulf(n, -2.0f * vec2_dot(n, a)));
}

inline Vec2 vec2_projectNorm(Vec2 a, Vec2 n)
{
	return vec2_mulf(n, vec2_dot(a, n));
}

inline Vec2 vec2_mirror(Vec2 a, Vec2 n)
{
	return vec2_sub(a, vec2_mulf(n, vec2_dot(a, n) * 2.0f));
}

inline Vec2 vec2_snap(Vec2 a, float size)
{
	a.x = snap(a.x, size);
	a.y = snap(a.y, size);
	return a;
}

inline Vec2 vec2_clamp(Vec2 a, Vec2 min, Vec2 max)
{
	a.x = clamp(a.x, -min.x, max.x);
	a.y = clamp(a.y, -min.y, max.y);
	return a;
}

inline Vec2 vec2_clampf(Vec2 a, float min, float max)
{
	float sz = clamp(vec2_mag(a), min, max);
	return vec2_mulf(vec2_norm(a), sz);
}

inline Vec2 vec2_rec(Vec2 a)
{
	if (nearEq(a.x, 0))
		a.x = MAX;
	else
		a.x = 1.0f / a.x;

	if (nearEq(a.y, 0))
		a.y = MAX;
	else
		a.y = 1.0f / a.y;

	return a;
}

inline float vec2_angle(Vec2 a, Vec2 b)
{
	float de = vec2_sqrMag(a) * vec2_sqrMag(b);
	if (de <= EPSILON2)
		return 0.0f;
	return acosd(clamp(vec2_dot(a, b) * invSqrt(de), -1.0f, 1.0f));
}

inline float vec2_cosAngle(Vec2 a, Vec2 b)
{
	a = vec2_norm(a);
	b = vec2_norm(b);
	return vec2_dot(a, b);
}

// vec3

inline Vec3 vec3(float x, float y, float z)
{
	Vec3 a;
	a.x = x;
	a.y = y;
	a.z = z;
	return a;
}

inline Vec3 vec3f(float a)
{
	Vec3 v;
	v.x = v.y = v.z = a;
	return v;
}

inline Vec3 vec3_neg(Vec3 a)
{
	a.x *= -1.0f;
	a.y *= -1.0f;
	a.z *= -1.0f;
	return a;
}

inline Vec3 vec3_add(Vec3 a, Vec3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline Vec3 vec3_addf(Vec3 a, float b)
{
	a.x += b;
	a.y += b;
	a.z += b;
	return a;
}

inline Vec3 vec3_sub(Vec3 a, Vec3 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline Vec3 vec3_subf(Vec3 a, float b)
{
	a.x -= b;
	a.y -= b;
	a.z -= b;
	return a;
}

inline Vec3 vec3_mulf(Vec3 a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}

inline float vec3_dot(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float vec3_sqrMag(Vec3 a)
{
	return vec3_dot(a, a);
}

inline float vec3_mag(Vec3 a)
{
	return sqrtf(vec3_dot(a, a));
}

inline float vec3_dist(Vec3 a, Vec3 b)
{
	return vec3_mag(vec3_sub(a, b));
}

inline float vec3_dot2d(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y;
}

inline float vec3_sqrMag2d(Vec3 a)
{
	return vec3_dot2d(a, a);
}

inline float vec3_mag2d(Vec3 a)
{
	return sqrtf(vec3_dot2d(a, a));
}

inline float vec3_dist2d(Vec3 a, Vec3 b)
{

	return vec3_mag2d(vec3_sub(a, b));
}

inline char vec3_eq(Vec3 a, Vec3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline char vec3_eq0(Vec3 a)
{
	return a.x == 0 && a.y == 0 && a.z == 0;
}

inline char vec3_nearEq(Vec3 a, Vec3 b)
{
	return vec3_dist(a, b) < EPSILON;
}

inline char vec3_near0(Vec3 a)
{
	return vec3_nearEq(a, vec3_zero);
}

inline Vec3 vec3_cross(Vec3 a, Vec3 b)
{
	Vec3 c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

inline Vec3 vec3_min(Vec3 a, Vec3 b)
{
	a.x = fminf(a.x, b.x);
	a.y = fminf(a.y, b.y);
	a.z = fminf(a.z, b.z);
	return a;
}

inline float vec3_minc(Vec3 a)
{
	return fminf(fminf(a.x, a.y), a.z);
}

inline Vec3 vec3_max(Vec3 a, Vec3 b)
{
	a.x = fmaxf(a.x, b.x);
	a.y = fmaxf(a.y, b.y);
	a.z = fmaxf(a.z, b.z);
	return a;
}

inline float vec3_maxc(Vec3 a)
{
	return fmaxf(fmaxf(a.x, a.y), a.z);
}

inline Vec3 vec3_abs(Vec3 a)
{
	a.x = fabsf(a.x);
	a.y = fabsf(a.y);
	a.z = fabsf(a.z);
	return a;
}

inline float vec3_absMin(Vec3 a)
{
	return fminf(fminf(fabsf(a.x), fabsf(a.y)), fabsf(a.z));
}

inline float vec3_absMax(Vec3 a)
{
	return fmaxf(fmaxf(fabsf(a.x), fabsf(a.y)), fabsf(a.z));
}

inline Vec3 vec3_norm(Vec3 a)
{
	float s = vec3_sqrMag(a);
	if (s == 1.0f)
		return a;
	if (s <= EPSILON2)
		return vec3_zero;
	float si = invSqrt(s);
	a.x *= si;
	a.y *= si;
	a.z *= si;
	return a;
}

inline Vec3 vec3_norm2d(Vec3 a)
{
	float s = vec3_sqrMag2d(a);
	if (s == 1)
	{
		if (a.z == 0)
			return a;
		a.z = 0;
		return a;
	}
	if (s <= EPSILON2)
		return vec3_zero;

	float si = invSqrt(s);
	a.x *= si;
	a.y *= si;
	a.z = 0;
	return a;
}

inline Vec3 vec3_sign(Vec3 a)
{
	a.x = sign(a.x);
	a.y = sign(a.y);
	a.z = sign(a.z);
	return a;
}

inline Vec3 vec3_lerp(Vec3 a, Vec3 b, float dt)
{
	a.x = a.x + (b.x - a.x) * dt;
	a.y = a.y + (b.y - a.y) * dt;
	a.z = a.z + (b.z - a.z) * dt;
	return a;
}

inline Vec3 vec3_lerp01(Vec3 a, Vec3 b, float dt)
{
	return vec3_lerp(a, b, clamp01(dt));
}

inline Vec3 vec3_reflect(Vec3 a, Vec3 n)
{
	return vec3_add(a, vec3_mulf(n, -2.0f * vec3_dot(n, a)));
}

inline Vec3 vec3_projection(Vec3 a)
{
	float rz = 1.0f / a.z;
	a.x = a.x * rz;
	a.y = a.y * rz;
	a.z = 0;
	return a;
}

inline Vec3 vec3_project(Vec3 a, Vec3 b)
{
	float s = vec3_sqrMag(b);
	if (s <= EPSILON2)
		return vec3_zero;
	return vec3_mulf(b, vec3_dot(a, b) * invSqrt(s));
}

inline Vec3 vec3_projectNorm(Vec3 a, Vec3 n)
{
	return vec3_mulf(n, vec3_dot(a, n));
}

inline Vec3 vec3_mirror(Vec3 a, Vec3 n)
{
	return vec3_sub(a, vec3_mulf(n, vec3_dot(a, n) * 2.0f));
}

inline Vec3 vec3_snap(Vec3 a, float size)
{
	a.x = snap(a.x, size);
	a.y = snap(a.y, size);
	a.z = snap(a.z, size);
	return a;
}

inline Vec3 vec3_clamp(Vec3 a, Vec3 min, Vec3 max)
{
	a.x = clamp(a.x, -min.x, max.x);
	a.y = clamp(a.y, -min.y, max.y);
	a.z = clamp(a.z, -min.z, max.z);
	return a;
}

inline Vec3 vec3_clampf(Vec3 a, float min, float max)
{
	float sz = clamp(vec3_mag(a), min, max);
	return vec3_mulf(vec3_norm(a), sz);
}

inline Vec3 vec3_clampf2d(Vec3 a, float min, float max)
{
	float z = a.z;
	a = vec3_mulf(vec3_norm(a), clamp(vec3_mag2d(a), min, max));
	a.z = z;
	return a;
}

inline Vec3 vec3_rec(Vec3 a)
{
	if (nearEq(a.x, 0))
		a.x = MAX;
	else
		a.x = 1.0f / a.x;

	if (nearEq(a.y, 0))
		a.y = MAX;
	else
		a.y = 1.0f / a.y;

	if (nearEq(a.z, 0))
		a.z = MAX;
	else
		a.z = 1.0f / a.z;

	return a;
}

inline float vec3_angle(Vec3 a, Vec3 b)
{
	float de = vec3_sqrMag(a) * vec3_sqrMag(b);
	if (de <= EPSILON2)
		return 0.0f;
	return acosd(clamp(vec3_dot(a, b) * invSqrt(de), -1.0f, 1.0f));
}

inline float vec3_signedAngle(Vec3 a, Vec3 b, Vec3 axis)
{
	return vec3_angle(a, b) * sign(vec3_dot(axis, vec3_cross(a, b)));
}

inline float vec3_cosAngle2d(Vec3 a, Vec3 b)
{
	a.z = 0;
	b.z = 0;
	a = vec3_norm(a);
	b = vec3_norm(b);
	return vec3_dot(a, b);
}

inline Vec3 vec3_intersectPlane(Vec3 a, Vec3 b, Vec3 o, Vec3 n)
{
	b = vec3_sub(b, a);
	return vec3_add(a, vec3_mulf(b, vec3_dot(vec3_sub(o, a), n) / vec3_dot(b, n)));
}

// plane

inline Plane plane(float x, float y, float z, float w)
{
	Plane p;
	p.x = x;
	p.y = y;
	p.z = z;
	p.w = w;
	return p;
}

// bbox

inline BBox bbox(Vec3 a, Vec3 b)
{
	BBox bb;
	bb.a = a;
	bb.b = b;
	return bb;
}

inline void bbox_vertices(const BBox *bbox, Vec3 out_vertices[8])
{
	out_vertices[0] = bbox->a;
	out_vertices[1] = vec3(bbox->a.x, bbox->b.y, bbox->a.z);
	out_vertices[2] = vec3(bbox->b.x, bbox->b.y, bbox->a.z);
	out_vertices[3] = vec3(bbox->b.x, bbox->a.y, bbox->a.z);
	out_vertices[4] = vec3(bbox->a.x, bbox->a.y, bbox->b.z);
	out_vertices[5] = vec3(bbox->a.x, bbox->b.y, bbox->b.z);
	out_vertices[6] = bbox->b;
	out_vertices[7] = vec3(bbox->b.x, bbox->a.y, bbox->b.z);
}

// vec4

inline Vec4 vec4(float x, float y, float z, float w)
{
	Vec4 a;
	a.x = x;
	a.y = y;
	a.z = z;
	a.w = w;
	return a;
}

// rot

inline Rot rot(float pitch, float yaw, float roll)
{
	Rot r;
	r.pitch = pitch;
	r.yaw = yaw;
	r.roll = roll;
	return r;
}
inline char rot_eq(Rot a, Rot b)
{
	return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll;
}

inline char rot_eq0(Rot a)
{
	return a.pitch == 0 && a.yaw == 0 && a.roll == 0;
}

inline char rot_nearEq(Rot a, Rot b)
{
	return nearEq(a.pitch, b.pitch) && nearEq(a.yaw, b.yaw) & nearEq(a.roll, b.roll);
}

inline char rot_near0(Rot a)
{
	return rot_nearEq(a, rot_zero);
}

inline Rot rot_add(Rot a, Rot b)
{
	a.pitch += b.pitch;
	a.yaw += b.yaw;
	a.roll += b.roll;
	return a;
}

inline Rot rot_addf(Rot a, float b)
{
	a.pitch += b;
	a.yaw += b;
	a.roll += b;
	return a;
}

inline Rot rot_sub(Rot a, Rot b)
{
	a.pitch -= b.pitch;
	a.yaw -= b.yaw;
	a.roll -= b.roll;
	return a;
}

inline Rot rot_subf(Rot a, float b)
{
	a.pitch -= b;
	a.yaw -= b;
	a.roll -= b;
	return a;
}

inline Rot rot_mulf(Rot a, float b)
{
	a.pitch *= b;
	a.yaw *= b;
	a.roll *= b;
	return a;
}

inline Rot rot_neg(Rot a)
{
	a.pitch *= -1.0f;
	a.yaw *= -1.0f;
	a.roll *= -1.0f;
	return a;
}

inline Rot rot_snap(Rot a, float size)
{
	a.pitch = snap(a.pitch, size);
	a.yaw = snap(a.yaw, size);
	a.roll = snap(a.roll, size);
	return a;
}

inline Rot rot_clamp(Rot a)
{
	a.pitch = clampAxis(a.pitch);
	a.yaw = clampAxis(a.yaw);
	a.roll = clampAxis(a.roll);
	return a;
}

inline char rot_nan(Rot a)
{
	return !finite(a.pitch) || !finite(a.yaw) || !finite(a.roll);
}

inline Rot rot_norm(Rot a)
{
	a.pitch = normAxis(a.pitch);
	a.yaw = normAxis(a.yaw);
	a.roll = normAxis(a.roll);
	return a;
}

inline Rot rot_deNorm(Rot a)
{
	a.pitch = clampAxis(a.pitch);
	a.yaw = clampAxis(a.yaw);
	a.roll = clampAxis(a.roll);
	return a;
}

inline float rot_dist(Rot a, Rot b)
{
	return fabsf(a.pitch - b.pitch) + fabsf(a.yaw - b.yaw) + fabsf(a.roll - b.roll);
}

inline Rot rot_eqv(Rot a)
{
	a.pitch = 180 - a.pitch;
	a.yaw = 180 + a.yaw;
	a.roll = 180 + a.roll;
	return a;
}

inline Rot rot_lerp(Rot a, Rot b, float dt)
{
	Rot n = rot_norm(rot_sub(b, a));
	a.pitch = a.pitch + n.pitch * dt;
	a.yaw = a.yaw + n.yaw * dt;
	a.roll = a.roll + n.roll * dt;
	return a;
}

inline Rot rot_lerp01(Rot a, Rot b, float dt)
{
	return rot_lerp(a, b, clamp01(dt));
}

inline Rot rot_rlerp(Rot a, Rot b, float dt)
{
	a.pitch = a.pitch + (b.pitch - a.pitch) * dt;
	a.yaw = a.yaw + (b.yaw - a.yaw) * dt;
	a.roll = a.roll + (b.roll - a.roll) * dt;
	return rot_norm(a);
}

inline Mat4 rot_matrix(Rot a, Vec3 origin)
{
	float cp = cosd(a.pitch);
	float sp = sind(a.pitch);
	float cy = cosd(a.yaw);
	float sy = sind(a.yaw);
	float cr = cosd(a.roll);
	float sr = sind(a.roll);
	Mat4 m;

	m.m[0][0] = cp * cy;
	m.m[0][1] = cp * sy;
	m.m[0][2] = sp;
	m.m[0][3] = 0.0f;

	m.m[1][0] = sr * sp * cy - cr * sy;
	m.m[1][1] = sr * sp * sy + cr * cy;
	m.m[1][2] = -sr * cp;
	m.m[1][3] = 0.0f;

	m.m[2][0] = -(cr * sp * cy + sr * sy);
	m.m[2][1] = cy * sr - cr * sp * sy;
	m.m[2][2] = cr * cp;
	m.m[2][3] = 0.0f;

	m.m[3][0] = origin.x;
	m.m[3][1] = origin.y;
	m.m[3][2] = origin.z;
	m.m[3][3] = 1.0f;

	return m;
}

inline Vec3 rot_forward(Rot a)
{
	float pitch = fmodf(a.pitch, 360.0f);
	float yaw = fmodf(a.yaw, 360.0f);
	float cp = cosd(pitch);
	float sp = sind(pitch);
	float cy = cosd(yaw);
	float sy = sind(yaw);
	Vec3 v;
	v.x = cp * cy;
	v.y = cp * sy;
	v.z = sp;
	return v;
}

inline Quat rot_quat(Rot a)
{
	float pitch = fmodf(a.pitch, 360.0f);
	float yaw = fmodf(a.yaw, 360.0f);
	float roll = fmodf(a.roll, 360.0f);

	float cp = cosd(pitch);
	float sp = sind(pitch);
	float cy = cosd(yaw);
	float sy = sind(yaw);
	float cr = cosd(roll);
	float sr = sind(roll);

	Quat q;
	q.x = cr * sp * sy - sr * cp * cy;
	q.y = -cr * sp * cy - sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;
	q.w = cr * cp * cy + sr * sp * sy;

	return q;
}

// quat

inline Quat quat(float pitch, float yaw, float roll)
{
	Rot r;
	r.pitch = pitch;
	r.yaw = yaw;
	r.roll = roll;
	return rot_quat(r);
}

inline Quat quat_mul(Quat a, Quat b)
{

	Quat q;
	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	return q;
}

inline Quat quat_mulf(Quat a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}

inline float quat_dot(Quat a, Quat b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
inline float quat_sqrmagnitude(Quat a) { return quat_dot(a, a); }
inline float quat_magnitude(Quat a) { return sqrtf(quat_sqrmagnitude(a)); }

inline Quat quat_normalize(Quat a)
{
	float b = quat_sqrmagnitude(a);
	if (b <= EPSILON2)
		return quat_identity;
	float si = invSqrt(b);
	a.x *= si;
	a.y *= si;
	a.z *= si;
	a.w *= si;
	return a;
}

inline Vec3 quat_unrotate(Quat q, Vec3 b)
{
	Vec3 a = {-q.x, -q.y, -q.z};
	Vec3 cross = vec3_mulf(vec3_cross(a, b), 2.0f);
	b = vec3_add(b, vec3_mulf(cross, q.w));
	b = vec3_add(b, vec3_cross(a, cross));
	return b;
}

inline Vec3 quat_rotate(Quat q, Vec3 b)
{
	Vec3 a = {q.x, q.y, q.z};
	Vec3 cross = vec3_mulf(vec3_cross(a, b), 2.0f);
	b = vec3_add(b, vec3_mulf(cross, -q.w));
	b = vec3_add(b, vec3_cross(a, cross));
	return b;
}

inline Vec3 quat_forward(Quat q)
{
	return quat_rotate(q, vec3_forward);
}

inline Vec3 quat_right(Quat q)
{
	return quat_rotate(q, vec3_right);
}

inline Vec3 quat_up(Quat q)
{
	return quat_rotate(q, vec3_up);
}

// mat4

inline Mat4 mat4(float a)
{

	Mat4 m = {{
		{a, 0, 0, 0},
		{0, a, 0, 0},
		{0, 0, a, 0},
		{0, 0, 0, a},
	}};
	return m;
}

inline Mat4 mat4_tran(Mat4 a)
{
	Mat4 m;
	m.m[0][0] = a.m[0][0];
	m.m[0][1] = a.m[1][0];
	m.m[0][2] = a.m[2][0];
	m.m[0][3] = a.m[3][0];
	m.m[1][0] = a.m[0][1];
	m.m[1][1] = a.m[1][1];
	m.m[1][2] = a.m[2][1];
	m.m[1][3] = a.m[3][1];
	m.m[2][0] = a.m[0][2];
	m.m[2][1] = a.m[1][2];
	m.m[2][2] = a.m[2][2];
	m.m[2][3] = a.m[3][2];
	m.m[3][0] = a.m[0][3];
	m.m[3][1] = a.m[1][3];
	m.m[3][2] = a.m[2][3];
	m.m[3][3] = a.m[3][3];
	return m;
}
inline Mat4 mat4_mul(Mat4 a, Mat4 b)
{

	Mat4 m;
#if defined(USE_AVX_256) || defined(USE_AVX_128)
	__m128 tmp, sum;
	__m128 b0 = _mm_loadu_ps(b.m[0]);
	__m128 b1 = _mm_loadu_ps(b.m[1]);
	__m128 b2 = _mm_loadu_ps(b.m[2]);
	__m128 b3 = _mm_loadu_ps(b.m[3]);

	tmp = _mm_set_ps1(a.m[0][0]);
	sum = _mm_mul_ps(tmp, b0);
	tmp = _mm_set_ps1(a.m[0][1]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
	tmp = _mm_set_ps1(a.m[0][2]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
	tmp = _mm_set_ps1(a.m[0][3]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
	_mm_storeu_ps(m.m[0], sum);

	tmp = _mm_set_ps1(a.m[1][0]);
	sum = _mm_mul_ps(tmp, b0);
	tmp = _mm_set_ps1(a.m[1][1]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
	tmp = _mm_set_ps1(a.m[1][2]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
	tmp = _mm_set_ps1(a.m[1][3]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
	_mm_storeu_ps(m.m[1], sum);

	tmp = _mm_set_ps1(a.m[2][0]);
	sum = _mm_mul_ps(tmp, b0);
	tmp = _mm_set_ps1(a.m[2][1]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
	tmp = _mm_set_ps1(a.m[2][2]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
	tmp = _mm_set_ps1(a.m[2][3]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
	_mm_storeu_ps(m.m[2], sum);

	tmp = _mm_set_ps1(a.m[3][0]);
	sum = _mm_mul_ps(tmp, b0);
	tmp = _mm_set_ps1(a.m[3][1]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
	tmp = _mm_set_ps1(a.m[3][2]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
	tmp = _mm_set_ps1(a.m[3][3]);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
	_mm_storeu_ps(m.m[3], sum);
#else
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m.m[i][j] = 0;
			for (int k = 0; k < 4; ++k)
			{
				m.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
#endif
	return m;
}

inline Mat4 mat4_mul3(Mat4 a, Mat4 b, Mat4 c)
{
	return mat4_mul(mat4_mul(a, b), c);
}

inline Mat4 mat4_add(Mat4 a, Mat4 b)
{
#if defined(USE_AVX_256)
	__m256 a0, b0;
	a0 = _mm256_loadu_ps(a.m[0]);
	b0 = _mm256_loadu_ps(b.m[0]);
	a0 = _mm256_add_ps(a0, b0);
	_mm256_storeu_ps(a.m[0], a0);

	a0 = _mm256_loadu_ps(a.m[2]);
	b0 = _mm256_loadu_ps(b.m[2]);
	a0 = _mm256_add_ps(a0, b0);
	_mm256_storeu_ps(a.m[2], a0);
#elif defined(USE_AVX_128)
	__m128 a0, b0;
	a0 = _mm_loadu_ps(a.m[0]);
	b0 = _mm_loadu_ps(b.m[0]);
	a0 = _mm_add_ps(a0, b0);
	_mm_storeu_ps(a.m[0], a0);
	a0 = _mm_loadu_ps(a.m[1]);
	b0 = _mm_loadu_ps(b.m[1]);
	a0 = _mm_add_ps(a0, b0);
	_mm_storeu_ps(a.m[1], a0);
	a0 = _mm_loadu_ps(a.m[2]);
	b0 = _mm_loadu_ps(b.m[2]);
	a0 = _mm_add_ps(a0, b0);
	_mm_storeu_ps(a.m[2], a0);
	a0 = _mm_loadu_ps(a.m[3]);
	b0 = _mm_loadu_ps(b.m[3]);
	a0 = _mm_add_ps(a0, b0);
	_mm_storeu_ps(a.m[3], a0);
#else
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 4; i++)
			a.m[j][i] += b.m[j][i];
#endif
	return a;
}

inline Mat4 mat4_sub(Mat4 a, Mat4 b)
{
#if defined(USE_AVX_256)
	__m256 a0, b0;
	a0 = _mm256_loadu_ps(a.m[0]);
	b0 = _mm256_loadu_ps(b.m[0]);
	a0 = _mm256_sub_ps(a0, b0);
	_mm256_storeu_ps(a.m[0], a0);
	a0 = _mm256_loadu_ps(a.m[2]);
	b0 = _mm256_loadu_ps(b.m[2]);
	a0 = _mm256_sub_ps(a0, b0);
	_mm256_storeu_ps(a.m[2], a0);
#elif defined(USE_AVX_128)
	__m128 a0, b0;
	a0 = _mm_loadu_ps(a.m[0]);
	b0 = _mm_loadu_ps(b.m[0]);
	a0 = _mm_sub_ps(a0, b0);
	_mm_storeu_ps(a.m[0], a0);
	a0 = _mm_loadu_ps(a.m[1]);
	b0 = _mm_loadu_ps(b.m[1]);
	a0 = _mm_sub_ps(a0, b0);
	_mm_storeu_ps(a.m[1], a0);
	a0 = _mm_loadu_ps(a.m[2]);
	b0 = _mm_loadu_ps(b.m[2]);
	a0 = _mm_sub_ps(a0, b0);
	_mm_storeu_ps(a.m[2], a0);
	a0 = _mm_loadu_ps(a.m[3]);
	b0 = _mm_loadu_ps(b.m[3]);
	a0 = _mm_sub_ps(a0, b0);
	_mm_storeu_ps(a.m[3], a0);
#else
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 4; i++)
			a.m[j][i] -= b.m[j][i];
#endif
	return a;
}

inline Mat4 mat4_mulf(Mat4 a, float b)
{
#if defined(USE_AVX_256)
	__m256 a0, b0;
	b0 = _mm256_set1_ps(b);
	a0 = _mm256_loadu_ps(a.m[0]);
	a0 = _mm256_mul_ps(a0, b0);
	_mm256_storeu_ps(a.m[0], a0);
	a0 = _mm256_loadu_ps(a.m[2]);
	a0 = _mm256_mul_ps(a0, b0);
	_mm256_storeu_ps(a.m[2], a0);
#elif defined(USE_AVX_128)
	__m128 a0, b0;
	b0 = _mm_set1_ps(b);
	a0 = _mm_loadu_ps(a.m[0]);
	a0 = _mm_mul_ps(a0, b0);
	_mm_storeu_ps(a.m[0], a0);
	a0 = _mm_loadu_ps(a.m[1]);
	a0 = _mm_mul_ps(a0, b0);
	_mm_storeu_ps(a.m[1], a0);
	a0 = _mm_loadu_ps(a.m[2]);
	a0 = _mm_mul_ps(a0, b0);
	_mm_storeu_ps(a.m[2], a0);
	a0 = _mm_loadu_ps(a.m[3]);
	a0 = _mm_mul_ps(a0, b0);
	_mm_storeu_ps(a.m[3], a0);
#else
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 4; i++)
			a.m[j][i] *= b;
#endif
	return a;
}

inline char mat4_eq(Mat4 a, Mat4 b)
{
	for (char j = 0; j < 4; j++)
		for (char i = 0; i < 4; i++)
			if (a.m[j][i] != b.m[j][i])
				return 0;
	return 1;
}

inline char mat4_nearEq(Mat4 a, Mat4 b)
{

	for (char j = 0; j < 4; j++)
		for (char i = 0; i < 4; i++)
			if (!nearEq(a.m[j][i], b.m[j][i]))
				return 0;
	return 1;
}

inline Vec4 mat4_mulv4(Mat4 a, Vec4 b)
{
#if defined(USE_AVX_256) || defined(USE_AVX_128)

	__m128 tmp, sum;
	__m128 b0 = _mm_loadu_ps(a.m[0]);
	__m128 b1 = _mm_loadu_ps(a.m[1]);
	__m128 b2 = _mm_loadu_ps(a.m[2]);
	__m128 b3 = _mm_loadu_ps(a.m[3]);

	tmp = _mm_set_ps1(b.x);
	sum = _mm_mul_ps(tmp, b0);
	tmp = _mm_set_ps1(b.y);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
	tmp = _mm_set_ps1(b.z);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
	tmp = _mm_set_ps1(b.w);
	sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
	_mm_storeu_ps((float *)(&b), sum);
	return b;

#else
#endif
	return b;
}

inline Vec3 mat4_mulv3(Mat4 a, Vec3 b, float w)
{
	Vec4 v = {b.x, b.y, b.z, w};
	v = mat4_mulv4(a, v);
	b.x = v.x;
	b.y = v.y;
	b.z = v.z;
	return b;
}

inline Mat4 mat4_scale(Vec3 a)
{
	Mat4 m;

	m.m[0][0] = a.x;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = a.y;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = a.z;
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}
inline Mat4 mat4_scalef(float a)
{

	Mat4 m;

	m.m[0][0] = a;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = a;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = a;
	m.m[2][3] = 0.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}

inline char mat4_containsNaN(Mat4 a)
{
	for (char j = 0; j < 4; j++)
		for (char i = 0; i < 4; i++)
			if (!finite(a.m[j][i]))
				return 0;
	return 1;
}

inline Mat4 mat4_origin(Vec3 a)
{
	Mat4 m;
	m.m[0][0] = 1.0f;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 1.0f;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = 1.0f;
	m.m[2][3] = 0.0f;

	m.m[3][0] = a.x;
	m.m[3][1] = a.y;
	m.m[3][2] = a.z;
	m.m[3][3] = 1.0f;

	return m;
}

inline Vec3 mat4_axis(const Mat4 *a, UnitAxisEnum ax)
{
	Vec3 b;
	switch (ax)
	{
	case UNIT_AXIS_X:
		b.x = a->m[0][0];
		b.y = a->m[0][1];
		b.z = a->m[0][2];
		return b;

	case UNIT_AXIS_Y:
		b.x = a->m[1][0];
		b.y = a->m[1][1];
		b.z = a->m[1][2];
		return b;

	case UNIT_AXIS_Z:
		b.x = a->m[2][0];
		b.y = a->m[2][1];
		b.z = a->m[2][2];
		return b;
	}
	return vec3_zero;
}
inline void mat4_axes(const Mat4 *a, Vec3 *ax, Vec3 *ay, Vec3 *az)
{
	ax->x = a->m[0][0];
	ax->y = a->m[0][1];
	ax->z = a->m[0][2];
	ay->x = a->m[1][0];
	ay->y = a->m[1][1];
	ay->z = a->m[1][2];
	az->x = a->m[2][0];
	az->y = a->m[2][1];
	az->z = a->m[2][2];
}

inline Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far)
{
	Mat4 m;

	m.m[0][0] = 2.0f / (right - left);
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = 2.0f / (top - bottom);
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = -1.0f / (far - near);
	m.m[2][3] = 0.0f;

	m.m[3][0] = -(right + left) / (right - left);
	m.m[3][1] = -(top + bottom) / (top - bottom);
	m.m[3][2] = -(far + near) / (far - near);
	m.m[3][3] = 1.0f;

	return m;
}

inline Mat4 mat4_perspective(float fov, float aspect, float near, float far)
{

	float t = tand(fov * 0.5f);
	float inv = 1.0f / (far - near);

	Mat4 m;
	m.m[0][0] = 1.0f / t;
	m.m[0][1] = 0.0f;
	m.m[0][2] = 0.0f;
	m.m[0][3] = 0.0f;

	m.m[1][0] = 0.0f;
	m.m[1][1] = aspect / t;
	m.m[1][2] = 0.0f;
	m.m[1][3] = 0.0f;

	m.m[2][0] = 0.0f;
	m.m[2][1] = 0.0f;
	m.m[2][2] = far * inv;
	m.m[2][3] = 1.0f;

	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = -near * far * inv;
	m.m[3][3] = 0.0f;

	return m;
}

inline Mat4 mat4_lookat(Vec3 eye, Vec3 center, Vec3 up)
{
	Mat4 m;
	Vec3 zaxis = vec3_norm(vec3_sub(center, eye));
	Vec3 xaxis = vec3_norm(vec3_cross(up, zaxis));
	Vec3 yaxis = vec3_cross(zaxis, xaxis);

	m.m[0][0] = xaxis.x;
	m.m[1][0] = xaxis.y;
	m.m[2][0] = xaxis.z;
	m.m[3][0] = -vec3_dot(eye, xaxis);
	m.m[0][1] = yaxis.x;
	m.m[1][1] = yaxis.y;
	m.m[2][1] = yaxis.z;
	m.m[3][1] = -vec3_dot(eye, yaxis);
	m.m[0][2] = zaxis.x;
	m.m[1][2] = zaxis.y;
	m.m[2][2] = zaxis.z;
	m.m[3][2] = -vec3_dot(eye, zaxis);
	m.m[0][3] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}

inline Vec3 rot_right(Rot a)
{
	Mat4 m = rot_matrix(a, vec3_zero);
	return mat4_axis(&m, UNIT_AXIS_Y);
}

inline Vec3 rot_up(Rot a)
{
	Mat4 m = rot_matrix(a, vec3_zero);
	return mat4_axis(&m, UNIT_AXIS_Z);
}

inline Vec3 rot_rotate(Rot r, Vec3 b)
{
	return mat4_mulv3(rot_matrix(r, vec3_zero), b, 1);
}

inline Vec3 rot_unrotate(Rot r, Vec3 b)
{
	return mat4_mulv3(mat4_tran(rot_matrix(r, vec3_zero)), b, 1);
}

inline Mat4 mat4_vec3(Vec3 xa, Vec3 ya, Vec3 za, Vec3 wa)
{
	Mat4 m;
	m.m[0][0] = xa.x;
	m.m[0][1] = xa.y;
	m.m[0][2] = xa.z;
	m.m[0][3] = 0.0f;

	m.m[1][0] = ya.x;
	m.m[1][1] = ya.y;
	m.m[1][2] = ya.z;
	m.m[1][3] = 0.0f;

	m.m[2][0] = za.x;
	m.m[2][1] = za.y;
	m.m[2][2] = za.z;
	m.m[2][3] = 0.0f;

	m.m[3][0] = wa.x;
	m.m[3][1] = wa.y;
	m.m[3][2] = wa.z;
	m.m[3][3] = 1.0f;
	return m;
}

inline Mat4 mat4_plane2(Plane a, Plane b, Plane c, Plane d)
{
	Mat4 m;
	m.m[0][0] = a.x;
	m.m[0][1] = a.y;
	m.m[0][2] = a.z;
	m.m[0][3] = a.w;
	m.m[1][0] = b.x;
	m.m[1][1] = b.y;
	m.m[1][2] = b.z;
	m.m[1][3] = b.w;
	m.m[2][0] = c.x;
	m.m[2][1] = c.y;
	m.m[2][2] = c.z;
	m.m[2][3] = c.w;
	m.m[3][0] = d.x;
	m.m[3][1] = d.y;
	m.m[3][2] = d.z;
	m.m[3][3] = d.w;
	return m;
}

inline Rot mat4_rot(Mat4 m)
{
	Vec3 ax, ay, az;
	mat4_axes(&m, &ax, &ay, &az);

	Rot r = rot(
		atan2d(ax.z, sqrtf(square(ax.x) + square(ax.y))),
		atan2d(ax.y, ax.x),
		0);

	Mat4 m2 = rot_matrix(r, vec3_zero);
	Vec3 say = mat4_axis(&m2, UNIT_AXIS_Y);
	r.roll = atan2d(vec3_dot(az, say), vec3_dot(ay, say));
	return r;
}

inline Mat4 mat4_invRot(Rot a)
{
	float cy = cosd(a.yaw);
	float sy = sind(a.yaw);
	float cp = cosd(a.pitch);
	float sp = sind(a.pitch);
	float cr = cosd(a.roll);
	float sr = sind(a.roll);
	Mat4 ma = {{
		{+cy, -sy, 0.f, 0.f},
		{+sy, +cy, 0.f, 0.f},
		{0.f, 0.f, 1.f, 0.f},
		{0.f, 0.f, 0.f, 1.f},
	}};
	Mat4 mb = {{
		{+cp, 0.f, -sp, 0.f},
		{0.f, 1.f, 0.f, 0.f},
		{+sp, 0.f, +cp, 0.f},
		{0.f, 0.f, 0.f, 1.f},
	}};
	Mat4 mc = {{
		{1.f, 0.f, 0.f, 0.f},
		{0.f, +cr, -sr, 0.f},
		{0.f, +sr, +cr, 0.f},
		{0.f, 0.f, 0.f, 1.f},
	}};
	Mat4 m = mat4_mul3(ma, mb, mc);
	return m;
}

inline Mat4 mat4_fromX(Vec3 x)
{
	x = vec3_norm(x);
	Vec3 up = (fabsf(x.z) < (1.f - EPSILON)) ? vec3(0, 0, 1.0f) : vec3(1.0f, 0, 0);
	Vec3 y = vec3_norm(vec3_cross(up, x));
	Vec3 z = vec3_cross(x, y);
	return mat4_vec3(x, y, z, vec3_zero);
}

inline Mat4 mat4_fromY(Vec3 y)
{
	y = vec3_norm(y);
	Vec3 up = (fabsf(y.z) < (1.f - EPSILON)) ? vec3(0, 0, 1.0f) : vec3(1.0f, 0, 0);
	Vec3 z = vec3_norm(vec3_cross(up, y));
	Vec3 x = vec3_cross(y, z);
	return mat4_vec3(x, y, z, vec3_zero);
}

inline Mat4 mat4_fromZ(Vec3 z)
{
	z = vec3_norm(z);
	Vec3 up = (fabsf(z.z) < (1.f - EPSILON)) ? vec3(0, 0, 1.0f) : vec3(1.0f, 0, 0);
	Vec3 x = vec3_norm(vec3_cross(up, z));
	Vec3 y = vec3_cross(z, x);
	return mat4_vec3(x, y, z, vec3_zero);
}

inline Mat4 mat4_view(Vec3 a, Rot b)
{
	Mat4 ma = {
		{{0, 0, 1, 0},
		 {1, 0, 0, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 1}}};
	return mat4_mul(
		mat4_origin(vec3_neg(a)),
		mat4_mul(mat4_invRot(b), ma));
}

inline Mat4 mat4_inv(Mat4 m)
{

	float da[4];
	Mat4 tmp, res;

	tmp.m[0][0] = m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2];
	tmp.m[0][1] = m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2];
	tmp.m[0][2] = m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2];

	tmp.m[1][0] = m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2];
	tmp.m[1][1] = m.m[0][2] * m.m[3][3] - m.m[0][3] * m.m[3][2];
	tmp.m[1][2] = m.m[0][2] * m.m[2][3] - m.m[0][3] * m.m[2][2];

	tmp.m[2][0] = m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2];
	tmp.m[2][1] = m.m[0][2] * m.m[3][3] - m.m[0][3] * m.m[3][2];
	tmp.m[2][2] = m.m[0][2] * m.m[1][3] - m.m[0][3] * m.m[1][2];

	tmp.m[3][0] = m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2];
	tmp.m[3][1] = m.m[0][2] * m.m[2][3] - m.m[0][3] * m.m[2][2];
	tmp.m[3][2] = m.m[0][2] * m.m[1][3] - m.m[0][3] * m.m[1][2];

	da[0] = m.m[1][1] * tmp.m[0][0] - m.m[2][1] * tmp.m[0][1] + m.m[3][1] * tmp.m[0][2];
	da[1] = m.m[0][1] * tmp.m[1][0] - m.m[2][1] * tmp.m[1][1] + m.m[3][1] * tmp.m[1][2];
	da[2] = m.m[0][1] * tmp.m[2][0] - m.m[1][1] * tmp.m[2][1] + m.m[3][1] * tmp.m[2][2];
	da[3] = m.m[0][1] * tmp.m[3][0] - m.m[1][1] * tmp.m[3][1] + m.m[2][1] * tmp.m[3][2];

	const float det = m.m[0][0] * da[0] - m.m[1][0] * da[1] + m.m[2][0] * da[2] - m.m[3][0] * da[3];
	const float rdet = 1.0f / det;

	res.m[0][0] = rdet * da[0];
	res.m[0][1] = -rdet * da[1];
	res.m[0][2] = rdet * da[2];
	res.m[0][3] = -rdet * da[3];
	res.m[1][0] = -rdet * (m.m[1][0] * tmp.m[0][0] - m.m[2][0] * tmp.m[0][1] + m.m[3][0] * tmp.m[0][2]);
	res.m[1][1] = rdet * (m.m[0][0] * tmp.m[1][0] - m.m[2][0] * tmp.m[1][1] + m.m[3][0] * tmp.m[1][2]);
	res.m[1][2] = -rdet * (m.m[0][0] * tmp.m[2][0] - m.m[1][0] * tmp.m[2][1] + m.m[3][0] * tmp.m[2][2]);
	res.m[1][3] = rdet * (m.m[0][0] * tmp.m[3][0] - m.m[1][0] * tmp.m[3][1] + m.m[2][0] * tmp.m[3][2]);
	res.m[2][0] = rdet * (m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) - m.m[2][0] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1]) + m.m[3][0] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1]));
	res.m[2][1] = -rdet * (m.m[0][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) - m.m[2][0] * (m.m[0][1] * m.m[3][3] - m.m[0][3] * m.m[3][1]) + m.m[3][0] * (m.m[0][1] * m.m[2][3] - m.m[0][3] * m.m[2][1]));
	res.m[2][2] = rdet * (m.m[0][0] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1]) - m.m[1][0] * (m.m[0][1] * m.m[3][3] - m.m[0][3] * m.m[3][1]) + m.m[3][0] * (m.m[0][1] * m.m[1][3] - m.m[0][3] * m.m[1][1]));
	res.m[2][3] = -rdet * (m.m[0][0] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1]) - m.m[1][0] * (m.m[0][1] * m.m[2][3] - m.m[0][3] * m.m[2][1]) + m.m[2][0] * (m.m[0][1] * m.m[1][3] - m.m[0][3] * m.m[1][1]));
	res.m[3][0] = -rdet * (m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) - m.m[2][0] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1]) + m.m[3][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]));
	res.m[3][1] = rdet * (m.m[0][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) - m.m[2][0] * (m.m[0][1] * m.m[3][2] - m.m[0][2] * m.m[3][1]) + m.m[3][0] * (m.m[0][1] * m.m[2][2] - m.m[0][2] * m.m[2][1]));
	res.m[3][2] = -rdet * (m.m[0][0] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1]) - m.m[1][0] * (m.m[0][1] * m.m[3][2] - m.m[0][2] * m.m[3][1]) + m.m[3][0] * (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]));
	res.m[3][3] = rdet * (m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) - m.m[1][0] * (m.m[0][1] * m.m[2][2] - m.m[0][2] * m.m[2][1]) + m.m[2][0] * (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]));

	return res;
}