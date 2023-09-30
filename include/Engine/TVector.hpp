#pragma once

#include <immintrin.h>
#include <utility>

extern "C" {
#include "mathf.h"
}


#define TVector_Zero (TVector(0, 0, 0))
#define TVector_One (TVector(1, 1, 1))
#define TVector_Forward (TVector(1, 0, 0))
#define TVector_Backward (TVector(-1, 0, 0))
#define TVector_Right (TVector(0, 1, 0))
#define TVector_Left (TVector(0, -1, 0))
#define TVector_Up (TVector(0, 0, 1))
#define TVector_Down (TVector(0, 0, -1))


struct __attribute__((aligned(16), packed)) TVector {
    float X{0};
    float Y{0};
    float Z{0};

    explicit inline TVector() : X(0), Y(0), Z(0) {}

    explicit inline TVector(const float &a) : X(a), Y(a), Z(a) {}

    explicit inline TVector(const float &x, const float &y) : X(x), Y(y), Z(0) {}

    explicit inline TVector(const float &x, const float &y, const float &z) : X(x), Y(y), Z(z) {}

    explicit inline TVector(const Vec3 &cVector) : X(cVector.x), Y(cVector.y), Z(cVector.z) {}

    explicit inline TVector(Vec3 &&cVector) : X(cVector.x), Y(cVector.y), Z(cVector.z) {}

    // operator equal

    inline TVector &operator=(const TVector &other) {
        if (this != &other) {
            X = other.X;
            Y = other.Y;
            Z = other.Z;
        }
        return *this;
    }

    inline TVector &operator=(TVector &&other) noexcept {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        return *this;
    }

    inline TVector &operator=(const float &other) {
        X = other;
        Y = other;
        Z = other;
        return *this;
    }

    inline TVector &operator=(const Vec3 &other) {
        X = other.x;
        Y = other.y;
        Z = other.z;
        return *this;
    }

    inline TVector &operator=(Vec3 &&other) {
        X = other.x;
        Y = other.y;
        Z = other.z;
        return *this;
    }

    // operator add

    inline friend TVector operator+(const TVector &a, const TVector &b) {
        return TVector(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
    }

    inline friend TVector operator+(const TVector &a, const float &b) {
        return TVector(a.X + b, a.Y + b, a.Z + b);
    }

    inline friend TVector operator+(const float &a, const TVector &b) {
        return TVector(a + b.X, a + b.Y, a + b.Z);
    }

    inline TVector &operator+=(const TVector &other) {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    inline TVector &operator+=(const float &other) {
        X += other;
        Y += other;
        Z += other;
        return *this;
    }

    // operator minus

    inline friend TVector operator-(const TVector &a, const TVector &b) {
        return TVector(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
    }

    inline friend TVector operator-(const TVector &a, const float &b) {
        return TVector(a.X - b, a.Y - b, a.Z - b);
    }

    inline friend TVector operator-(const float &a, const TVector &b) {
        return TVector(a - b.X, a - b.Y, a - b.Z);
    }

    inline TVector &operator-=(const TVector &other) {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        return *this;
    }

    inline TVector &operator-=(const float &other) {
        X -= other;
        Y -= other;
        Z -= other;
        return *this;
    }

    // operator negate

    inline friend TVector operator-(const TVector &a) {
        return TVector(-a.X, -a.Y, -a.Z);
    }

    // operator multiply

    inline friend TVector operator*(const TVector &a, const TVector &b) {
        return TVector(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
    }

    inline friend TVector operator*(const TVector &a, const float &b) {
        return TVector(a.X * b, a.Y * b, a.Z * b);
    }

    inline friend TVector operator*(const float &a, const TVector &b) {
        return TVector(a * b.X, a * b.Y, a * b.Z);
    }

    inline TVector &operator*=(const TVector &other) {
        X *= other.X;
        Y *= other.Y;
        Z *= other.Z;
        return *this;
    }

    inline TVector &operator*=(const float &other) {
        X *= other;
        Y *= other;
        Z *= other;
        return *this;
    }


    // operator divide

    inline friend TVector operator/(const TVector &a, const TVector &b) {
        return TVector(
                b.X == 0 ? MAX : a.X / b.X,
                b.Y == 0 ? MAX : a.Y / b.Y,
                b.Z == 0 ? MAX : a.Z / b.Z
        );
    }

    inline friend TVector operator/(const TVector &a, const float &b) {
        if (b == 0) return TVector(MAX);
        return TVector(a.X / b, a.Y / b, a.Z / b);
    }

    inline friend TVector operator/(const float &a, const TVector &b) {
        return TVector(
                b.X == 0 ? MAX : a / b.X,
                b.Y == 0 ? MAX : a / b.Y,
                b.Z == 0 ? MAX : a / b.Z
        );
    }

    inline TVector &operator/=(const TVector &other) {
        X = other.X == 0 ? MAX : X / other.X;
        Y = other.Y == 0 ? MAX : Y / other.Y;
        Z = other.Z == 0 ? MAX : Z / other.Z;
        return *this;
    }

    inline TVector &operator/=(const float &other) {
        if (other == 0) {
            X = MAX;
            Y = MAX;
            Z = MAX;
            return *this;
        }
        X /= other;
        Y /= other;
        Z /= other;
        return *this;
    }

    inline float &operator[](int i) {
        assert(i >= 0 && i <= 2 && "TVector: invalid parameter index");
        if (i == 0)
            return *(&X);
        if (i == 1)
            return *(&Y);
        if (i == 2)
            return *(&Z);
    }


    // operator dot
    inline friend float operator|(const TVector &a, const TVector &b) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }

    // operator cross
    inline friend TVector operator^(const TVector &a, const TVector &b) {
        return TVector(
                a.Y * b.Z - a.Z * b.Y,
                a.Z * b.X - a.X * b.Z,
                a.X * b.Y - a.Y * b.X
        );
    }

    // a == b
    inline friend bool operator==(const TVector &a, const TVector &b) {
        return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
    }

    inline friend bool operator==(const float &a, const TVector &b) {
        return a == b.X && a == b.Y && a == b.Z;
    }

    inline friend bool operator==(const TVector &a, const float &b) {
        return a.X == b && a.Y == b && a.Z == b;
    }

    // a != b
    inline friend bool operator!=(const TVector &a, const TVector &b) {
        return a.X != b.X || a.Y != b.Y || a.Z == b.Z;
    }

    inline friend bool operator!=(const float &a, const TVector &b) {
        return a != b.X || a != b.Y || a == b.Z;
    }

    inline friend bool operator!=(const TVector &a, const float &b) {
        return a.X != b || a.Y != b || a.Z == b;
    }

    // a > b
    inline friend bool operator>(const TVector &a, const TVector &b) {
        return a.X > b.X && a.Y > b.Y && a.Z > b.Z;
    }

    inline friend bool operator>(const float &a, const TVector &b) {
        return a > b.X && a > b.Y && a > b.Z;
    }

    inline friend bool operator>(const TVector &a, const float &b) {
        return a.X > b && a.Y > b && a.Z > b;
    }

    // a < b
    inline friend bool operator<(const TVector &a, const TVector &b) {
        return a.X < b.X && a.Y < b.Y && a.Z < b.Z;
    }

    inline friend bool operator<(const float &a, const TVector &b) {
        return a < b.X && a < b.Y && a < b.Z;
    }

    inline friend bool operator<(const TVector &a, const float &b) {
        return a.X < b && a.Y < b && a.Z < b;
    }

    // a >= b
    inline friend bool operator>=(const TVector &a, const TVector &b) {
        return a.X >= b.X && a.Y >= b.Y && a.Z >= b.Z;
    }

    inline friend bool operator>=(const float &a, const TVector &b) {
        return a >= b.X && a >= b.Y && a >= b.Z;
    }

    inline friend bool operator>=(const TVector &a, const float &b) {
        return a.X >= b && a.Y >= b && a.Z >= b;
    }

    // a <= b
    inline friend bool operator<=(const TVector &a, const TVector &b) {
        return a.X <= b.X && a.Y <= b.Y && a.Z <= b.Z;
    }

    inline friend bool operator<=(const float &a, const TVector &b) {
        return a <= b.X && a <= b.Y && a <= b.Z;
    }

    inline friend bool operator<=(const TVector &a, const float &b) {
        return a.X <= b && a.Y <= b && a.Z <= b;
    }

    // near %=
    inline friend bool operator%=(const TVector &a, const TVector &b) {
        return nearEq(a.X, b.X) && nearEq(a.Y, b.Y) && nearEq(a.Z, b.Z);
    }

    inline friend bool operator%=(const float &a, const TVector &b) {
        return nearEq(a, b.X) && nearEq(a, b.Y) && nearEq(a, b.Z);
    }

    inline friend bool operator%=(const TVector &a, const float &b) {
        return nearEq(a.X, b) && nearEq(a.Y, b) && nearEq(a.Z, b);
    }

    // min max

    [[nodiscard]] [[maybe_unused]]
    inline friend TVector operator~(const TVector &a) {
        float s = a.SqrLength();
        if (s == 1.0f) return a.Clone();
        if (s <= EPSILON2) return TVector_Zero;
        return a * invSqrt(s);
    }

    //

    inline TVector &Normalize() {
        float s = SqrLength();
        if (s == 1.0f) return *this;
        if (s <= EPSILON2) return (*this = 0);
        return (*this *= invSqrt(s));
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Clone() const {
        return TVector(X, Y, Z);
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Normal() const {
        return ~(*this);
    }

    // operations

    [[nodiscard]] [[maybe_unused]]
    inline TVector Min(const TVector &b) const {
        return TVector(
                fminf(X, b.X),
                fminf(Y, b.Y),
                fminf(Z, b.Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Max(const TVector &b) const {
        return TVector(
                fmaxf(X, b.X),
                fmaxf(Y, b.Y),
                fmaxf(Z, b.Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Min() const {
        return fminf(fminf(X, Y), Z);
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Max() const {
        return fmaxf(fmaxf(X, Y), Z);
    }


    [[nodiscard]] [[maybe_unused]]
    inline float AbsMin() const {
        return fminf(fminf(fabsf(X), fabsf(Y)), fabsf(Z));
    }

    [[nodiscard]] [[maybe_unused]]
    inline float AbsMax() const {
        return fmaxf(fmaxf(fabsf(X), fabsf(Y)), fabsf(Z));
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Abs() const {
        return TVector(
                fabsf(X),
                fabsf(Y),
                fabsf(Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline float SqrLength() const {
        return X * X + Y * Y + Z * Z;
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Length() const {
        return sqrtf(X * X + Y * Y + Z * Z);
    }

    [[nodiscard]] [[maybe_unused]]
    inline float AngleTo(const TVector &other) const {
        float de = SqrLength() * other.SqrLength();
        if (de <= EPSILON2) return 0.0f;
        return acosd(clamp((*this | other) * invSqrt(de), -1.0f, 1.0f));
    }

    [[nodiscard]] [[maybe_unused]]
    inline float SignedAngleTo(const TVector &other, const TVector &axis) const {
        return AngleTo(other) * sign((axis | (*this ^ other)));
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector IntersectOnPlane(const TVector &direction, const TVector &origin, const TVector &normal) const {
        return (*this + (direction * ((origin - *this) | normal) / (direction | normal)));
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Distance(const TVector &other) const {
        return sqrtf(
                (X - other.X) * (X - other.X) +
                (Y - other.Y) * (Y - other.Y) +
                (Z - other.Z) * (Z - other.Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Sign() const {
        return TVector(
                sign(X),
                sign(Y),
                sign(Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Snap(float scale) const {
        return TVector(
                snap(X, scale),
                snap(Y, scale),
                snap(Z, scale)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector SnapCube(float scale) const {
        float hs = scale * 0.5f;
        return TVector(
                hs + snap(X - hs, scale),
                hs + snap(Y - hs, scale),
                hs + snap(Z - hs, scale)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Clamp(const TVector &min, const TVector &max) const {
        return TVector(
                clamp(X, min.X, max.X),
                clamp(Y, min.Y, max.Y),
                clamp(Z, min.Z, max.Z)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Clamp(float min, float max) const {
        return TVector(
                clamp(X, min, max),
                clamp(Y, min, max),
                clamp(Z, min, max)
        );
    }


    [[nodiscard]] [[maybe_unused]]
    inline TVector &SnapTo(float scale) {
        X = snap(X, scale);
        Y = snap(Y, scale);
        Z = snap(Z, scale);
        return *this;
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector &SnapToCube(float scale) {
        float hs = scale * 0.5f;
        X = hs + snap(X - hs, scale);
        Y = hs + snap(Y - hs, scale);
        Z = hs + snap(Z - hs, scale);
        return *this;
    }


    [[nodiscard]] [[maybe_unused]]
    inline TVector &ClampTo(const TVector &min, const TVector &max) {
        X = clamp(X, min.X, max.X);
        Y = clamp(Y, min.Y, max.Y);
        Z = clamp(Z, min.Z, max.Z);
        return *this;
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector &ClampTo(float min, float max) {
        X = clamp(X, min, max);
        Y = clamp(Y, min, max);
        Z = clamp(Z, min, max);
        return *this;
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector Reflect(const TVector &normal) const {
        return (*this + (-2.0f * (normal | *this)) * normal);
    }

    [[maybe_unused]]
    inline TVector &ProjectOn(const TVector &other) {
        float s = other.SqrLength();
        if (s <= EPSILON2) {
            return (*this = 0);
        }
        return (*this = (other * ((*this | other) * invSqrt(s))));
    }

    [[maybe_unused]]
    inline TVector &ProjectOnNormal(const TVector &normal) {
        return (*this = normal * (*this | normal));
    }


    [[maybe_unused]]
    inline TVector &MoveTowards(const TVector &destination, float maxDelta) {
        TVector toVector = destination - *this;
        float dist = toVector.Length();
        if (dist <= maxDelta || dist < EPSILON)
            return (*this = destination);
        else
            return (*this += toVector * (maxDelta / dist));
    }

    [[maybe_unused]]
    inline TVector &LerpTo(const TVector &destination, float deltaTime) {
        return (*this += (destination - *this) * deltaTime);
    }

    // static

    [[nodiscard]] [[maybe_unused]]
    inline static TVector MoveTowards(const TVector &current, const TVector &destination, float maxDelta) {
        TVector toVector = destination - current;
        float dist = toVector.Length();
        if (dist <= maxDelta || dist < EPSILON)
            return destination.Clone();
        return current + toVector * (maxDelta / dist);
    }

    [[nodiscard]] [[maybe_unused]]
    inline static TVector Lerp(const TVector &current, const TVector &destination, float maxDelta) {
        return current + (destination - current) * maxDelta;
    }

    [[nodiscard]] [[maybe_unused]]
    inline static TVector Lerp01(const TVector &current, const TVector &destination, float maxDelta) {
        return current + (destination - current) * clamp01(maxDelta);
    }

    [[nodiscard]] [[maybe_unused]]
    inline static TVector Cross(const TVector &a, const TVector &b) {
        return a ^ b;
    }

    [[nodiscard]] [[maybe_unused]]
    inline static float Dot(const TVector &a, const TVector &b) {
        return a | b;
    }

    [[nodiscard]] [[maybe_unused]]
    inline static float Distance(const TVector &a, const TVector &b) {
        return a.Distance(b);
    }

    [[nodiscard]] [[maybe_unused]]
    inline static float Length(const TVector &a) {
        return a.Length();
    }

    [[nodiscard]] [[maybe_unused]]
    inline static float SqrLength(const TVector &a) {
        return a.SqrLength();
    }

    inline static TVector Random(const TVector &scale) {
        return TVector(
                randf() * scale.X * 2 - scale.X,
                randf() * scale.Y * 2 - scale.Y,
                randf() * scale.Z * 2 - scale.Z
        );
    }

    inline static TVector Random(const float &scale) {
        float doubleScale = scale * 2.0f;
        return TVector(
                randf() * doubleScale - scale,
                randf() * doubleScale - scale,
                randf() * doubleScale - scale
        );
    }

    // trace

    inline void Stream(std::ostream &os) const {
        os << "TVector(" << X << ", " << Y << ", " << Z << ")";
    }
};