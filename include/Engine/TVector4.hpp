#pragma once

#include <utility>
#include <immintrin.h>

extern "C" {
#include "mathf.h"
}

#include "engine/TVector.hpp"

struct __attribute__((aligned(16))) TVector4 {
    float X{0};
    float Y{0};
    float Z{0};
    float W{0};

    explicit inline TVector4() : X(0), Y(0), Z(0), W(0) {}

    explicit inline TVector4(const float &a) : X(a), Y(a), Z(a), W(a) {}

    explicit inline TVector4(const float &x, const float &y) : X(x), Y(y), Z(0), W(0) {}

    explicit inline TVector4(const float &x, const float &y, const float &z) : X(x), Y(y), Z(z), W(0) {}

    explicit inline TVector4(const float &x, const float &y, const float &z, const float &w) : X(x), Y(y), Z(z), W(w) {}

    explicit inline TVector4(const TVector4 &other) = default;

    inline TVector4(TVector4 &&other) noexcept: X(other.X), Y(other.Y), Z(other.Z), W(other.W) {}

    explicit inline TVector4(const TVector &other) : X(other.X), Y(other.Y), Z(other.Z), W(0) {};

    explicit inline TVector4(const TVector &other, const float &w) : X(other.X), Y(other.Y), Z(other.Z), W(w) {};

    inline TVector4(TVector &&other) noexcept: X(other.X), Y(other.Y), Z(other.Z), W(0) {}

    inline TVector4(TVector &&other, const float &w) noexcept: X(other.X), Y(other.Y), Z(other.Z), W(w) {}

    // operator equal

    inline TVector4 &operator=(const TVector4 &other) {
        if (this != &other) {
            X = other.X;
            Y = other.Y;
            Z = other.Z;
            W = other.W;
        }
        return *this;
    }

    inline TVector4 &operator=(TVector4 &&other) noexcept {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        W = other.W;
        return *this;
    }

    inline TVector4 &operator=(const TVector &other) {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        return *this;
    }

    inline TVector4 &operator=(TVector &&other) noexcept {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        return *this;
    }

    inline TVector4 &operator=(const float &other) {
        X = other;
        Y = other;
        Z = other;
        W = other;
        return *this;
    }

    // operator add

    inline friend TVector4 operator+(const TVector4 &a, const TVector4 &b) {
        return TVector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
    }

    inline friend TVector4 operator+(const TVector4 &a, const TVector &b) {
        return TVector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W);
    }

    inline friend TVector4 operator+(const TVector &a, const TVector4 &b) {
        return TVector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, b.W);
    }

    inline friend TVector4 operator+(const TVector4 &a, const float &b) {
        return TVector4(a.X + b, a.Y + b, a.Z + b, a.W + b);
    }

    inline friend TVector4 operator+(const float &a, const TVector4 &b) {
        return TVector4(a + b.X, a + b.Y, a + b.Z, a + b.W);
    }

    inline TVector4 &operator+=(const TVector4 &other) {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        W += other.W;
        return *this;
    }

    inline TVector4 &operator+=(const TVector &other) {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    inline TVector4 &operator+=(const float &other) {
        X += other;
        Y += other;
        Z += other;
        W += other;
        return *this;
    }

    // operator minus

    inline friend TVector4 operator-(const TVector4 &a, const TVector4 &b) {
        return TVector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W);
    }

    inline friend TVector4 operator-(const TVector4 &a, const TVector &b) {
        return TVector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W);
    }

    inline friend TVector4 operator-(const TVector &a, const TVector4 &b) {
        return TVector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, b.W);
    }

    inline friend TVector4 operator-(const TVector4 &a, const float &b) {
        return TVector4(a.X - b, a.Y - b, a.Z - b, a.W - b);
    }

    inline friend TVector4 operator-(const float &a, const TVector4 &b) {
        return TVector4(a - b.X, a - b.Y, a - b.Z, a - b.W);
    }

    inline TVector4 &operator-=(const TVector4 &other) {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        W -= other.W;
        return *this;
    }

    inline TVector4 &operator-=(const TVector &other) {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        return *this;
    }

    inline TVector4 &operator-=(const float &other) {
        X -= other;
        Y -= other;
        Z -= other;
        W -= other;
        return *this;
    }
    // operator negate

    inline friend TVector4 operator-(const TVector4 &a) {
        return TVector4(-a.X, -a.Y, -a.Z, -a.W);
    }

    // operator multiply

    inline friend TVector4 operator*(const TVector4 &a, const TVector4 &b) {
        return TVector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.W * b.W);
    }

    inline friend TVector4 operator*(const TVector4 &a, const TVector &b) {
        return TVector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.W);
    }

    inline friend TVector4 operator*(const TVector &a, const TVector4 &b) {
        return TVector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, b.W);
    }

    inline friend TVector4 operator*(const TVector4 &a, const float &b) {
        return TVector4(a.X * b, a.Y * b, a.Z * b, a.W * b);
    }

    inline friend TVector4 operator*(const float &a, const TVector4 &b) {
        return TVector4(a * b.X, a * b.Y, a * b.Z, a * b.W);
    }

    inline TVector4 &operator*=(const TVector4 &other) {
        X *= other.X;
        Y *= other.Y;
        Z *= other.Z;
        W *= other.W;
        return *this;
    }

    inline TVector4 &operator*=(const float &other) {
        X *= other;
        Y *= other;
        Z *= other;
        W *= other;
        return *this;
    }


    // operator divide

    inline friend TVector4 operator/(const TVector4 &a, const TVector4 &b) {
        return TVector4(
                b.X == 0 ? MAX : a.X / b.X,
                b.Y == 0 ? MAX : a.Y / b.Y,
                b.Z == 0 ? MAX : a.Z / b.Z,
                b.W == 0 ? MAX : a.W / b.W
        );
    }

    inline friend TVector4 operator/(const TVector4 &a, const float &b) {
        if (b == 0) return TVector4(MAX);
        return TVector4(a.X / b, a.Y / b, a.Z / b, a.W / b);
    }

    inline friend TVector4 operator/(const float &a, const TVector4 &b) {
        return TVector4(
                b.X == 0 ? MAX : a / b.X,
                b.Y == 0 ? MAX : a / b.Y,
                b.Z == 0 ? MAX : a / b.Z,
                b.W == 0 ? MAX : a / b.W
        );
    }

    inline friend TVector4 operator/(const TVector4 &a, const TVector &b) {
        if (b == 0) return TVector4(MAX);
        return TVector4(a.X / b.X, a.Y / b.Y, a.Z / b.Z, a.W);
    }

    inline friend TVector4 operator/(const TVector &a, const TVector4 &b) {
        return TVector4(
                b.X == 0 ? MAX : a.X / b.X,
                b.Y == 0 ? MAX : a.Y / b.Y,
                b.Z == 0 ? MAX : a.Z / b.Z,
                b.W == 0 ? MAX : b.W
        );
    }

    inline TVector4 &operator/=(const TVector4 &other) {
        X = other.X == 0 ? MAX : X / other.X;
        Y = other.Y == 0 ? MAX : Y / other.Y;
        Z = other.Z == 0 ? MAX : Z / other.Z;
        W = other.W == 0 ? MAX : W / other.W;
        return *this;
    }

    inline TVector4 &operator/=(const TVector &other) {
        X = other.X == 0 ? MAX : X / other.X;
        Y = other.Y == 0 ? MAX : Y / other.Y;
        Z = other.Z == 0 ? MAX : Z / other.Z;
        return *this;
    }

    inline TVector4 &operator/=(const float &other) {
        if (other == 0) {
            X = MAX;
            Y = MAX;
            Z = MAX;
            W = MAX;
            return *this;
        }
        X /= other;
        Y /= other;
        Z /= other;
        W /= other;
        return *this;
    }

    inline float &operator[](int i) {
        assert(i >= 0 && i <= 3 && "TVector4: invalid parameter index");
        if (i == 0)
            return X;
        if (i == 1)
            return Y;
        if (i == 2)
            return Z;
        if (i == 3)
            return W;
    }


    // operator dot
    inline friend float operator|(const TVector4 &a, const TVector4 &b) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
    }

    // a == b
    inline friend bool operator==(const TVector4 &a, const TVector4 &b) {
        return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
    }

    inline friend bool operator==(const float &a, const TVector4 &b) {
        return a == b.X && a == b.Y && a == b.Z && a == b.W;
    }

    inline friend bool operator==(const TVector4 &a, const float &b) {
        return a.X == b && a.Y == b && a.Z == b && a.W == b;
    }

    // a != b
    inline friend bool operator!=(const TVector4 &a, const TVector4 &b) {
        return a.X != b.X || a.Y != b.Y || a.Z == b.Z || a.W == b.W;
    }

    inline friend bool operator!=(const float &a, const TVector4 &b) {
        return a != b.X || a != b.Y || a == b.Z || a == b.W;
    }

    inline friend bool operator!=(const TVector4 &a, const float &b) {
        return a.X != b || a.Y != b || a.Z == b || a.W == b;
    }

    // a > b
    inline friend bool operator>(const TVector4 &a, const TVector4 &b) {
        return a.X > b.X && a.Y > b.Y && a.Z > b.Z && a.W > b.W;
    }

    inline friend bool operator>(const float &a, const TVector4 &b) {
        return a > b.X && a > b.Y && a > b.Z && a > b.W;
    }

    inline friend bool operator>(const TVector4 &a, const float &b) {
        return a.X > b && a.Y > b && a.Z > b && a.W > b;
    }

    // a < b
    inline friend bool operator<(const TVector4 &a, const TVector4 &b) {
        return a.X < b.X && a.Y < b.Y && a.Z < b.Z && a.W < b.W;
    }

    inline friend bool operator<(const float &a, const TVector4 &b) {
        return a < b.X && a < b.Y && a < b.Z && a < b.W;
    }

    inline friend bool operator<(const TVector4 &a, const float &b) {
        return a.X < b && a.Y < b && a.Z < b && a.W < b;
    }

    // a >= b
    inline friend bool operator>=(const TVector4 &a, const TVector4 &b) {
        return a.X >= b.X && a.Y >= b.Y && a.Z >= b.Z && a.W >= b.W;
    }

    inline friend bool operator>=(const float &a, const TVector4 &b) {
        return a >= b.X && a >= b.Y && a >= b.Z && a >= b.W;
    }

    inline friend bool operator>=(const TVector4 &a, const float &b) {
        return a.X >= b && a.Y >= b && a.Z >= b && a.W >= b;
    }

    // a <= b
    inline friend bool operator<=(const TVector4 &a, const TVector4 &b) {
        return a.X <= b.X && a.Y <= b.Y && a.Z <= b.Z && a.W <= b.W;
    }

    inline friend bool operator<=(const float &a, const TVector4 &b) {
        return a <= b.X && a <= b.Y && a <= b.Z && a <= b.W;
    }

    inline friend bool operator<=(const TVector4 &a, const float &b) {
        return a.X <= b && a.Y <= b && a.Z <= b && a.W <= b;
    }

    // near %=
    inline friend bool operator%=(const TVector4 &a, const TVector4 &b) {
        return nearEq(a.X, b.X) && nearEq(a.Y, b.Y) && nearEq(a.Z, b.Z) && nearEq(a.W, b.W);
    }

    inline friend bool operator%=(const float &a, const TVector4 &b) {
        return nearEq(a, b.X) && nearEq(a, b.Y) && nearEq(a, b.Z) && nearEq(a, b.W);
    }

    inline friend bool operator%=(const TVector4 &a, const float &b) {
        return nearEq(a.X, b) && nearEq(a.Y, b) && nearEq(a.Z, b) && nearEq(a.W, b);
    }

    // min max

    [[nodiscard]] [[maybe_unused]]
    inline friend TVector4 operator~(const TVector4 &a) {
        float s = a.SqrLength();
        if (s == 1.0f) return a.Clone();
        if (s <= EPSILON2) return TVector();
        return a * invSqrt(s);
    }

    //

    inline TVector4 &Normalize() {
        float s = SqrLength();
        if (s == 1.0f) return *this;
        if (s <= EPSILON2) return (*this = 0);
        return (*this *= invSqrt(s));
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Clone() const {
        return TVector4(X, Y, Z, W);
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Normal() const {
        return ~(*this);
    }

    // operations

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Min(const TVector4 &b) const {
        return TVector4(
                fminf(X, b.X),
                fminf(Y, b.Y),
                fminf(Z, b.Z),
                fminf(W, b.W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Max(const TVector4 &b) const {
        return TVector4(
                fmaxf(X, b.X),
                fmaxf(Y, b.Y),
                fmaxf(Z, b.Z),
                fmaxf(W, b.W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Min() const {
        return fminf(fminf(X, Y), fminf(Z, W));
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Max() const {
        return fmaxf(fmaxf(X, Y), fmaxf(Z, W));
    }


    [[nodiscard]] [[maybe_unused]]
    inline float AbsMin() const {
        return fminf(fminf(fabsf(X), fabsf(Y)), fminf(fabsf(Z), fabsf(W)));
    }

    [[nodiscard]] [[maybe_unused]]
    inline float AbsMax() const {
        return fmaxf(fmaxf(fabsf(X), fabsf(Y)), fmaxf(fabsf(Z), fabsf(W)));
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Abs() const {
        return TVector4(
                fabsf(X),
                fabsf(Y),
                fabsf(Z),
                fabsf(W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline float SqrLength() const {
        return X * X + Y * Y + Z * Z + W * W;
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Length() const {
        return sqrtf(X * X + Y * Y + Z * Z);
    }

    [[nodiscard]] [[maybe_unused]]
    inline float Distance(const TVector4 &other) const {
        return sqrtf(
                (X - other.X) * (X - other.X) +
                (Y - other.Y) * (Y - other.Y) +
                (Z - other.Z) * (Z - other.Z) +
                (W - other.W) * (W - other.W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Sign() const {
        return TVector4(
                sign(X),
                sign(Y),
                sign(Z),
                sign(W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Snap(float scale) const {
        return TVector4(
                snap(X, scale),
                snap(Y, scale),
                snap(Z, scale),
                snap(W, scale)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 SnapCube(float scale) const {
        float hs = scale * 0.5f;
        return TVector4(
                hs + snap(X - hs, scale),
                hs + snap(Y - hs, scale),
                hs + snap(Z - hs, scale),
                hs + snap(W - hs, scale)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Clamp(const TVector4 &min, const TVector4 &max) const {
        return TVector4(
                clamp(X, min.X, max.X),
                clamp(Y, min.Y, max.Y),
                clamp(Z, min.Z, max.Z),
                clamp(W, min.W, max.W)
        );
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 Clamp(float min, float max) const {
        return TVector4(
                clamp(X, min, max),
                clamp(Y, min, max),
                clamp(Z, min, max),
                clamp(W, min, max)
        );
    }


    [[nodiscard]] [[maybe_unused]]
    inline TVector4 &SnapTo(float scale) {
        X = snap(X, scale);
        Y = snap(Y, scale);
        Z = snap(Z, scale);
        W = snap(W, scale);
        return *this;
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 &SnapToCube(float scale) {
        float hs = scale * 0.5f;
        X = hs + snap(X - hs, scale);
        Y = hs + snap(Y - hs, scale);
        Z = hs + snap(Z - hs, scale);
        W = hs + snap(W - hs, scale);
        return *this;
    }


    [[nodiscard]] [[maybe_unused]]
    inline TVector4 &ClampTo(const TVector4 &min, const TVector4 &max) {
        X = clamp(X, min.X, max.X);
        Y = clamp(Y, min.Y, max.Y);
        Z = clamp(Z, min.Z, max.Z);
        W = clamp(W, min.W, max.W);
        return *this;
    }

    [[nodiscard]] [[maybe_unused]]
    inline TVector4 &ClampTo(float min, float max) {
        X = clamp(X, min, max);
        Y = clamp(Y, min, max);
        Z = clamp(Z, min, max);
        W = clamp(W, min, max);
        return *this;
    }

    // static

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

    inline static TVector4 Random(const TVector4 &scale) {
        return TVector4(
                randf() * scale.X * 2 - scale.X,
                randf() * scale.Y * 2 - scale.Y,
                randf() * scale.Z * 2 - scale.Z,
                randf() * scale.W * 2 - scale.W
        );
    }

    inline static TVector4 Random(const float &scale) {
        float doubleScale = scale * 2.0f;
        return TVector4(
                randf() * doubleScale - scale,
                randf() * doubleScale - scale,
                randf() * doubleScale - scale,
                randf() * doubleScale - scale
        );
    }

    // trace

    inline void Stream(std::ostream &os) const {
        os << "TVector4(" << X << ", " << Y << ", " << Z << ", " << W << ")";
    }
};