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

inline static bool operator!(const Vec3 &a) {
    return vec3_near0(a);
}
//

inline static Vec3 operator~(const Vec3 &a) {
    return vec3_norm(a);
}
// -------------------------------------------------------


// vec3
// operator (+)


inline static Vec3i operator+(const Vec3i &a, const Vec3i &b) {
    return Vec3i{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline static Vec3i operator+(const Vec3i &a, const int &b) {
    return Vec3i{a.x + b, a.y + b, a.z + b};
}

inline static Vec3i operator+(const int &a, const Vec3i &b) {
    return Vec3i{a + b.x, a + b.y, a + b.z};
}

inline static Vec3i &operator+=(Vec3i &current, const Vec3i &other) {
    current.x += other.x;
    current.y += other.y;
    current.z += other.z;
    return current;
}

inline static Vec3i &operator+=(Vec3i &current, const int &other) {
    current.x += other;
    current.y += other;
    current.z += other;
    return current;
}

// operator (-)
inline static Vec3i operator-(const Vec3i &a) {
    return Vec3i{-a.x, -a.y, -a.z};
}

inline static Vec3i operator-(const Vec3i &a, const Vec3i &b) {
    return Vec3i{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline static Vec3i operator-(const Vec3i &a, const int &b) {
    return Vec3i{a.x - b, a.y - b, a.z - b};
}

inline static Vec3i operator-(const int &a, const Vec3i &b) {
    return Vec3i{a - b.x, a - b.y, a - b.z};
}

inline static Vec3i &operator-=(Vec3i &current, const Vec3i &other) {
    current.x -= other.x;
    current.y -= other.y;
    current.z -= other.z;
    return current;
}

inline static Vec3i &operator-=(Vec3i &current, const int &other) {
    current.x -= other;
    current.y -= other;
    current.z -= other;
    return current;
}

// operator (*)

inline static Vec3i operator*(const Vec3i &a, const Vec3i &b) {
    return Vec3i{a.x * b.x, a.y * b.y, a.z * b.z};
}

inline static Vec3i operator*(const Vec3i &a, const int &b) {
    return Vec3i{a.x * b, a.y * b, a.z * b};
}

inline static Vec3i operator*(const int &a, const Vec3i &b) {
    return Vec3i{a * b.x, a * b.y, a * b.z};
}

inline static Vec3i &operator*=(Vec3i &current, const Vec3i &other) {
    current.x *= other.x;
    current.y *= other.y;
    current.z *= other.z;
    return current;
}

inline static Vec3i &operator*=(Vec3i &current, const int &other) {
    current.x *= other;
    current.y *= other;
    current.z *= other;
    return current;
}

// operator (/)

inline static Vec3i operator/(const Vec3i &a, const Vec3i &b) {
    return Vec3i{
            b.x == 0 ? INT_MAX : a.x / b.x,
            b.y == 0 ? INT_MAX : a.y / b.y,
            b.z == 0 ? INT_MAX : a.z / b.z
    };
}

inline static Vec3i operator/(const Vec3i &a, const int &b) {
    if (b == 0) return Vec3i{INT_MAX, INT_MAX, INT_MAX};
    return Vec3i{a.x / b, a.y / b, a.z / b};
}

inline static Vec3i operator/(const int &a, const Vec3i &b) {
    return Vec3i{
            b.x == 0 ? INT_MAX : a / b.x,
            b.y == 0 ? INT_MAX : a / b.y,
            b.z == 0 ? INT_MAX : a / b.z
    };
}

inline static Vec3i &operator/=(Vec3i &current, const Vec3i &other) {
    current.x = other.x == 0 ? INT_MAX : current.x / other.x;
    current.y = other.y == 0 ? INT_MAX : current.y / other.y;
    current.z = other.z == 0 ? INT_MAX : current.z / other.z;
    return current;
}

inline static Vec3i &operator/=(Vec3i &current, const int &other) {
    if (other == 0) {
        current.x = INT_MAX;
        current.y = INT_MAX;
        current.z = INT_MAX;
        return current;
    }
    current.x /= other;
    current.y /= other;
    current.z /= other;
    return current;
}

// -------------------------------------------------------


// vec3
// operator (+)

inline static Rot operator+(const Rot &a) {
    return Rot{fabs(a.pitch), fabs(a.yaw), fabs(a.roll)};
}

inline static Rot operator+(const Rot &a, const Rot &b) {
    return Rot{a.pitch + b.pitch, a.yaw + b.yaw, a.roll + b.roll};
}

inline static Rot operator+(const Rot &a, const float &b) {
    return Rot{a.pitch + b, a.yaw + b, a.roll + b};
}

inline static Rot operator+(const float &a, const Rot &b) {
    return Rot{a + b.pitch, a + b.yaw, a + b.roll};
}

inline static Rot &operator+=(Rot &current, const Rot &other) {
    current.pitch += other.pitch;
    current.yaw += other.yaw;
    current.roll += other.roll;
    return current;
}

inline static Rot &operator+=(Rot &current, const float &other) {
    current.pitch += other;
    current.yaw += other;
    current.roll += other;
    return current;
}

// operator (-)
inline static Rot operator-(const Rot &a) {
    return rot_inv(a);
}

inline static Rot operator-(const Rot &a, const Rot &b) {
    return Rot{a.pitch - b.pitch, a.yaw - b.yaw, a.roll - b.roll};
}

inline static Rot operator-(const Rot &a, const float &b) {
    return Rot{a.pitch - b, a.yaw - b, a.roll - b};
}

inline static Rot operator-(const float &a, const Rot &b) {
    return Rot{a - b.pitch, a - b.yaw, a - b.roll};
}

inline static Rot &operator-=(Rot &current, const Rot &other) {
    current.pitch -= other.pitch;
    current.yaw -= other.yaw;
    current.roll -= other.roll;
    return current;
}

inline static Rot &operator-=(Rot &current, const float &other) {
    current.pitch -= other;
    current.yaw -= other;
    current.roll -= other;
    return current;
}

// operator (*)

inline static Rot operator*(const Rot &a, const Rot &b) {
    return Rot{a.pitch * b.pitch, a.yaw * b.yaw, a.roll * b.roll};
}

inline static Rot operator*(const Rot &a, const float &b) {
    return Rot{a.pitch * b, a.yaw * b, a.roll * b};
}

inline static Rot operator*(const float &a, const Rot &b) {
    return Rot{a * b.pitch, a * b.yaw, a * b.roll};
}

inline static Rot &operator*=(Rot &current, const Rot &other) {
    current.pitch *= other.pitch;
    current.yaw *= other.yaw;
    current.roll *= other.roll;
    return current;
}

inline static Rot &operator*=(Rot &current, const float &other) {
    current.pitch *= other;
    current.yaw *= other;
    current.roll *= other;
    return current;
}

// operator (/)

inline static Rot operator/(const Rot &a, const Rot &b) {
    return Rot{
            b.pitch == 0 ? 0 : a.pitch / b.pitch,
            b.yaw == 0 ? 0 : a.yaw / b.yaw,
            b.roll == 0 ? 0 : a.roll / b.roll
    };
}

inline static Rot operator/(const Rot &a, const float &b) {
    if (b == 0) return Rot{0, 0, 0};
    return Rot{a.pitch / b, a.yaw / b, a.roll / b};
}

inline static Rot operator/(const float &a, const Rot &b) {
    return Rot{
            b.pitch == 0 ? 0 : a / b.pitch,
            b.yaw == 0 ? 0 : a / b.yaw,
            b.roll == 0 ? 0 : a / b.roll
    };
}

inline static Rot &operator/=(Rot &current, const Rot &other) {
    current.pitch = other.pitch == 0 ? 0 : current.pitch / other.pitch;
    current.yaw = other.yaw == 0 ? 0 : current.yaw / other.yaw;
    current.roll = other.roll == 0 ? 0 : current.roll / other.roll;
    return current;
}

inline static Rot &operator/=(Rot &current, const float &other) {
    if (other == 0) {
        current.pitch = 0;
        current.yaw = 0;
        current.roll = 0;
        return current;
    }
    current.pitch /= other;
    current.yaw /= other;
    current.roll /= other;
    return current;
}

// a == b
inline static bool operator==(const Rot &a, const Rot &b) {
    return rot_nearEq(a, b);
}

inline static bool operator==(const float &a, const Rot &b) {
    return nearEq(a, b.pitch) && nearEq(a, b.yaw) && nearEq(a, b.roll);
}

inline static bool operator==(const Rot &a, const float &b) {
    return nearEq(a.pitch, b) && nearEq(a.yaw, b) && nearEq(a.roll, b);
}

// a != b
inline static bool operator!=(const Rot &a, const Rot &b) {
    return !(a == b);
}

inline static bool operator!=(const float &a, const Rot &b) {
    return !(a == b);
}

inline static bool operator!=(const Rot &a, const float &b) {
    return !(a == b);
}

inline static bool operator!(const Rot &a) {
    return rot_near0(a);
}

inline static Rot operator~(const Rot &a) {
    return rot_norm(a);
}