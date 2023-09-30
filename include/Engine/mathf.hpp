#pragma once

#include <cassert>

extern "C" {
#include "mathf.h"
}

// triangle

inline static bool operator==(const Triangle &lhs, const Triangle &rhs) {
    return triangle_nearEq(lhs, rhs);
}

// vec3
// operator (+)

inline static Vec3 operator+(const Vec3 &a) {
    return Vec3{fabs(a.x), fabs(a.y), fabs(a.z)};
}

inline static Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline static Vec3 operator+(const Vec3 &a, const float &b) {
    return Vec3{a.x + b, a.y + b, a.z + b};
}

inline static Vec3 operator+(const float &a, const Vec3 &b) {
    return Vec3{a + b.x, a + b.y, a + b.z};
}

inline static Vec3 &operator+=(Vec3 &current, const Vec3 &other) {
    current.x += other.x;
    current.y += other.y;
    current.z += other.z;
    return current;
}

inline static Vec3 &operator+=(Vec3 &current, const float &other) {
    current.x += other;
    current.y += other;
    current.z += other;
    return current;
}

// operator (-)

inline static Vec3 operator-(const Vec3 &a) {
    return Vec3{-a.x, -a.y, -a.z};
}

inline static Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline static Vec3 operator-(const Vec3 &a, const float &b) {
    return Vec3{a.x - b, a.y - b, a.z - b};
}

inline static Vec3 operator-(const float &a, const Vec3 &b) {
    return Vec3{a - b.x, a - b.y, a - b.z};
}

inline static Vec3 &operator-=(Vec3 &current, const Vec3 &other) {
    current.x -= other.x;
    current.y -= other.y;
    current.z -= other.z;
    return current;
}

inline static Vec3 &operator-=(Vec3 &current, const float &other) {
    current.x -= other;
    current.y -= other;
    current.z -= other;
    return current;
}

// operator (*)

inline static Vec3 operator*(const Vec3 &a, const Vec3 &b) {
    return Vec3{a.x * b.x, a.y * b.y, a.z * b.z};
}

inline static Vec3 operator*(const Vec3 &a, const float &b) {
    return Vec3{a.x * b, a.y * b, a.z * b};
}

inline static Vec3 operator*(const float &a, const Vec3 &b) {
    return Vec3{a * b.x, a * b.y, a * b.z};
}

inline static Vec3 &operator*=(Vec3 &current, const Vec3 &other) {
    current.x *= other.x;
    current.y *= other.y;
    current.z *= other.z;
    return current;
}

inline static Vec3 &operator*=(Vec3 &current, const float &other) {
    current.x *= other;
    current.y *= other;
    current.z *= other;
    return current;
}

// operator (/)

inline static Vec3 operator/(const Vec3 &a, const Vec3 &b) {
    return Vec3{
            b.x == 0 ? MAX : a.x / b.x,
            b.y == 0 ? MAX : a.y / b.y,
            b.z == 0 ? MAX : a.z / b.z
    };
}

inline static Vec3 operator/(const Vec3 &a, const float &b) {
    if (b == 0) return Vec3{MAX, MAX, MAX};
    return Vec3{a.x / b, a.y / b, a.z / b};
}

inline static Vec3 operator/(const float &a, const Vec3 &b) {
    return Vec3{
            b.x == 0 ? MAX : a / b.x,
            b.y == 0 ? MAX : a / b.y,
            b.z == 0 ? MAX : a / b.z
    };
}

inline static Vec3 &operator/=(Vec3 &current, const Vec3 &other) {
    current.x = other.x == 0 ? MAX : current.x / other.x;
    current.y = other.y == 0 ? MAX : current.y / other.y;
    current.z = other.z == 0 ? MAX : current.z / other.z;
    return current;
}

inline static Vec3 &operator/=(Vec3 &current, const float &other) {
    if (other == 0) {
        current.x = MAX;
        current.y = MAX;
        current.z = MAX;
        return current;
    }
    current.x /= other;
    current.y /= other;
    current.z /= other;
    return current;
}

// operator dot
inline static float operator|(const Vec3 &a, const Vec3 &b) {
    return vec3_dot(a, b);
}

// operator cross
inline static Vec3 operator^(const Vec3 &a, const Vec3 &b) {
    return vec3_cross(a, b);
}

// a == b
inline static bool operator==(const Vec3 &a, const Vec3 &b) {
    return vec3_nearEq(a, b);
}

inline static bool operator==(const float &a, const Vec3 &b) {
    return nearEq(a, b.x) && nearEq(a, b.y) && nearEq(a, b.z);
}

inline static bool operator==(const Vec3 &a, const float &b) {
    return nearEq(a.x, b) && nearEq(a.y, b) && nearEq(a.z, b);
}

// a != b
inline static bool operator!=(const Vec3 &a, const Vec3 &b) {
    return !(a == b);
}

inline static bool operator!=(const float &a, const Vec3 &b) {
    return !(a == b);
}

inline static bool operator!=(const Vec3 &a, const float &b) {
    return !(a == b);
}
//

inline static Vec3 operator~(const Vec3 &a) {
    return vec3_norm(a);
}