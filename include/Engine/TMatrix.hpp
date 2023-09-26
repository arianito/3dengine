#pragma once

#include <cstring>

#include "engine/TVector.hpp"
#include "engine/TVector4.hpp"
#include "engine/TRotator.hpp"
#include "engine/TRect.hpp"

struct __attribute__((aligned(64))) TMatrix {
    float M[4][4]{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };

    explicit inline TMatrix() = default;

    explicit inline TMatrix(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
    ) : M{
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
    } {}

    explicit inline TMatrix(const float &scale) {
        M[0][0] = scale;
        M[1][1] = scale;
        M[2][2] = scale;
    }

    explicit inline TMatrix(const TMatrix &other) {
        memcpy(M, other.M, 16 * sizeof(float));
    }

    explicit inline TMatrix(const Mat4 &other) {
        memcpy(M, other.m, 16 * sizeof(float));
    }

    explicit inline TMatrix(const TRect &bounds, const float &nearPlane, const float &farPlane) {
        float fmd = farPlane - nearPlane;
        float fpd = farPlane + nearPlane;
        float tmb = bounds.MaxY - bounds.MinY;
        float tpb = bounds.MaxY + bounds.MinY;
        float rml = bounds.MaxX - bounds.MinX;
        float rpl = bounds.MaxX + bounds.MinX;
        M[0][0] = 2.0f / rml;
        M[1][1] = -2.0f / tmb;
        M[2][2] = -1.0f / fmd;
        M[3][0] = -rpl / rml;
        M[3][1] = tpb / tmb;
        M[3][2] = -fpd / fmd;
    }

    explicit inline TMatrix(const float &fov, const float &aspect, const float &nearPlane, const float &farPlane) {
        float t = tand(fov * 0.5f);
        float inv = 1.0f / (farPlane - nearPlane);
        M[0][0] = 1.0f / t;
        M[1][1] = aspect / t;
        M[2][2] = farPlane * inv;
        M[2][3] = 1.0f;
        M[3][2] = -(nearPlane * farPlane * inv);
        M[3][3] = 0.0f;
    }

    explicit inline TMatrix(const TVector &scale) {
        M[0][0] = scale.X;
        M[1][1] = scale.Y;
        M[2][2] = scale.Z;
    }


    inline TMatrix(TMatrix &&other) noexcept {
        std::swap(M, other.M);
    }

    explicit inline TMatrix(const TVector &vector, const int &axis) {
        assert(axis >= 0 && axis <= 2 && "TMatrix: invalid axis");
        auto up = (fabsf(vector.Z) < (1.f - EPSILON))
                  ? TVector_Up
                  : TVector_Forward;
        if (axis == 0) {
            auto x = ~vector;
            auto y = ~(up ^ x);
            auto z = x ^ y;
            SetAxes(x, y, z, TVector_Zero);
        } else if (axis == 1) {
            auto y = ~vector;
            auto z = ~(up ^ y);
            auto x = y ^ z;
            SetAxes(x, y, z, TVector_Zero);
        } else if (axis == 2) {
            auto z = ~vector;
            auto x = ~(up ^ z);
            auto y = z ^ x;
            SetAxes(x, y, z, TVector_Zero);
        }
    }


    explicit inline TMatrix(const TVector &position, const TRotator &rotation) {
        TMatrix ma(0, 0, 1, 0,
                   1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 0, 1);
        *this = TMatrix().SetOrigin(-position) * (TMatrix(rotation) * ma);
    }

    explicit inline TMatrix(const TVector &position, const TRotator &rotation, const TVector &scale) {
        *this = TMatrix(scale) * TMatrix(rotation, position);
    }

    explicit inline TMatrix(const TVector &eye, const TVector &center, const TVector &up) {
        TVector zAxis = ~(center - eye);
        TVector xAxis = ~(up ^ zAxis);
        TVector yAxis = zAxis ^ xAxis;

        M[0][0] = xAxis.X;
        M[1][0] = xAxis.Y;
        M[2][0] = xAxis.Z;
        M[3][0] = -(eye | xAxis);
        M[0][1] = yAxis.X;
        M[1][1] = yAxis.Y;
        M[2][1] = yAxis.Z;
        M[3][1] = -(eye | yAxis);
        M[0][2] = zAxis.X;
        M[1][2] = zAxis.Y;
        M[2][2] = zAxis.Z;
        M[3][2] = -(eye | zAxis);
    }

    explicit inline TMatrix(const TRotator &rotation, const TVector &origin) {
        float cp = cosd(rotation.Pitch);
        float sp = sind(rotation.Pitch);
        float cy = cosd(rotation.Yaw);
        float sy = sind(rotation.Yaw);
        float cr = cosd(rotation.Roll);
        float sr = sind(rotation.Roll);

        M[0][0] = +(cp * cy);
        M[0][1] = +(cp * sy);
        M[0][2] = +(sp);

        M[1][0] = -(cr * sy) + (sr * sp * cy);
        M[1][1] = +(cy * cr) + (sp * sy * sr);
        M[1][2] = -(cp * sr);

        M[2][0] = -(cy * cr * sp) - (sy * sr);
        M[2][1] = +(cy * sr) - (cr * sp * sy);
        M[2][2] = +(cp * cr);

        M[3][0] = origin.X;
        M[3][1] = origin.Y;
        M[3][2] = origin.Z;
    }

    explicit inline TMatrix(const TRotator &invRot) {
        float cp = cosd(invRot.Pitch);
        float sp = sind(invRot.Pitch);
        float cy = cosd(invRot.Yaw);
        float sy = sind(invRot.Yaw);
        float cr = cosd(invRot.Roll);
        float sr = sind(invRot.Roll);

        M[0][0] = +(cp * cy);
        M[0][1] = -(cr * sy) - (cy * sp * sr);
        M[0][2] = +(sy * sr) - (cy * cr * sp);

        M[1][0] = +(cy * sp);
        M[1][1] = +(cy * cr) - (sp * sy * sr);
        M[1][2] = -(cy * sr) - (cr * sp * sy);

        M[2][0] = +(sp);
        M[2][1] = +(cp * sr);
        M[2][2] = +(cp * cr);
    }

    explicit inline TMatrix(const TVector &xa, const TVector &ya, const TVector &za, const TVector &wa) {
        M[0][0] = xa.X;
        M[0][1] = xa.Y;
        M[0][2] = xa.Z;
        M[1][0] = ya.X;
        M[1][1] = ya.Y;
        M[1][2] = ya.Z;
        M[2][0] = za.X;
        M[2][1] = za.Y;
        M[2][2] = za.Z;
        M[3][0] = wa.X;
        M[3][1] = wa.Y;
        M[3][2] = wa.Z;

    }
    // operator assign

    inline TMatrix &operator=(const TMatrix &other) {
        if (this != &other) {
            memcpy(M, other.M, 16 * sizeof(float));
        }
        return *this;
    }

    inline TMatrix &operator=(TMatrix &&other) noexcept {
        std::swap(M, other.M);
        return *this;
    }

    inline TMatrix &operator=(const Mat4 &other) {
        memcpy(M, other.m, 16 * sizeof(float));
        return *this;
    }
    // a + b

    inline friend TMatrix operator+(const TMatrix &a, const TMatrix &b) {
        TMatrix result;
#if defined(USE_AVX_256)
        __m256 a0, b0;
        a0 = _mm256_loadu_ps(a.M[0]);
        b0 = _mm256_loadu_ps(b.M[0]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(result.M[0], a0);

        a0 = _mm256_loadu_ps(a.M[2]);
        b0 = _mm256_loadu_ps(b.M[2]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(result.M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        a0 = _mm_loadu_ps(a.M[0]);
        b0 = _mm_loadu_ps(b.M[0]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[0], a0);
        a0 = _mm_loadu_ps(a.M[1]);
        b0 = _mm_loadu_ps(b.M[1]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[1], a0);
        a0 = _mm_loadu_ps(a.M[2]);
        b0 = _mm_loadu_ps(b.M[2]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[2], a0);
        a0 = _mm_loadu_ps(a.M[3]);
        b0 = _mm_loadu_ps(b.M[3]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                result.M[j][i] = a.M[j][i] + b.M[j][i];
#endif
        return result;
    }

    inline TMatrix &operator+=(const TMatrix &b) {
#if defined(USE_AVX_256)
        __m256 a0, b0;
        a0 = _mm256_loadu_ps(M[0]);
        b0 = _mm256_loadu_ps(b.M[0]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(M[0], a0);

        a0 = _mm256_loadu_ps(M[2]);
        b0 = _mm256_loadu_ps(b.M[2]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        a0 = _mm_loadu_ps(M[0]);
        b0 = _mm_loadu_ps(b.M[0]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[0], a0);
        a0 = _mm_loadu_ps(M[1]);
        b0 = _mm_loadu_ps(b.M[1]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[1], a0);
        a0 = _mm_loadu_ps(M[2]);
        b0 = _mm_loadu_ps(b.M[2]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[2], a0);
        a0 = _mm_loadu_ps(M[3]);
        b0 = _mm_loadu_ps(b.M[3]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                M[j][i] += b.M[j][i];
#endif
        return *this;
    }



    //


    // a + b

    inline friend TMatrix operator-(const TMatrix &a, const TMatrix &b) {
        TMatrix result;
#if defined(USE_AVX_256)
        __m256 a0, b0;
        a0 = _mm256_loadu_ps(a.M[0]);
        b0 = _mm256_loadu_ps(b.M[0]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(result.M[0], a0);

        a0 = _mm256_loadu_ps(a.M[2]);
        b0 = _mm256_loadu_ps(b.M[2]);
        a0 = _mm256_add_ps(a0, b0);
        _mm256_storeu_ps(result.M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        a0 = _mm_loadu_ps(a.M[0]);
        b0 = _mm_loadu_ps(b.M[0]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[0], a0);
        a0 = _mm_loadu_ps(a.M[1]);
        b0 = _mm_loadu_ps(b.M[1]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[1], a0);
        a0 = _mm_loadu_ps(a.M[2]);
        b0 = _mm_loadu_ps(b.M[2]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[2], a0);
        a0 = _mm_loadu_ps(a.M[3]);
        b0 = _mm_loadu_ps(b.M[3]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                result.M[j][i] = a.M[j][i] + b.M[j][i];
#endif
        return result;
    }

    inline TMatrix &operator-=(const TMatrix &b) {
#if defined(USE_AVX_256)
        __m256 a0, b0;
        a0 = _mm256_loadu_ps(M[0]);
        b0 = _mm256_loadu_ps(b.M[0]);
        a0 = _mm256_sub_ps(a0, b0);
        _mm256_storeu_ps(M[0], a0);

        a0 = _mm256_loadu_ps(M[2]);
        b0 = _mm256_loadu_ps(b.M[2]);
        a0 = _mm256_sub_ps(a0, b0);
        _mm256_storeu_ps(M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        a0 = _mm_loadu_ps(M[0]);
        b0 = _mm_loadu_ps(b.M[0]);
        a0 = _mm_sub_ps(a0, b0);
        _mm_storeu_ps(M[0], a0);
        a0 = _mm_loadu_ps(M[1]);
        b0 = _mm_loadu_ps(b.M[1]);
        a0 = _mm_sub_ps(a0, b0);
        _mm_storeu_ps(M[1], a0);
        a0 = _mm_loadu_ps(M[2]);
        b0 = _mm_loadu_ps(b.M[2]);
        a0 = _mm_sub_ps(a0, b0);
        _mm_storeu_ps(M[2], a0);
        a0 = _mm_loadu_ps(M[3]);
        b0 = _mm_loadu_ps(b.M[3]);
        a0 = _mm_sub_ps(a0, b0);
        _mm_storeu_ps(M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                M[j][i] -= b.M[j][i];
#endif
        return *this;
    }

    //


    // *

    inline friend TMatrix operator*(const TMatrix &a, const float &b) {
        TMatrix result;
#if defined(USE_AVX_256)
        __m256 a0, b0;
        b0 = _mm256_set1_ps(b);

        a0 = _mm256_loadu_ps(a.M[0]);
        a0 = _mm256_mul_ps(a0, b0);
        _mm256_storeu_ps(result.M[0], a0);
        a0 = _mm256_loadu_ps(a.M[2]);
        a0 = _mm256_mul_ps(a0, b0);
        _mm256_storeu_ps(result.M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        b0 = _mm_set1_ps(b);
        a0 = _mm_loadu_ps(a.M[0]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[0], a0);
        a0 = _mm_loadu_ps(a.M[1]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[1], a0);
        a0 = _mm_loadu_ps(a.M[2]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[2], a0);
        a0 = _mm_loadu_ps(a.M[3]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(result.M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                result.M[j][i] = a.M[j][i] * b.M[j][i];
#endif
        return result;
    }

    inline friend TVector4 operator*(const TMatrix &a, const TVector4 &b) {
        TVector4 result;
#if defined(USE_AVX_256) || defined(USE_AVX_128)
        __m128 tmp, sum;
        __m128 b0 = _mm_loadu_ps(a.M[0]);
        __m128 b1 = _mm_loadu_ps(a.M[1]);
        __m128 b2 = _mm_loadu_ps(a.M[2]);
        __m128 b3 = _mm_loadu_ps(a.M[3]);

        tmp = _mm_set_ps1(b.X);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(b.Y);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(b.Z);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(b.W);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps((float *) (&result), sum);
#endif
        return result;
    }

    inline friend TMatrix operator*(const TMatrix &a, const TMatrix &b) {

        TMatrix m;
#if defined(USE_AVX_256) || defined(USE_AVX_128)
        __m128 tmp, sum;
        __m128 b0 = _mm_loadu_ps(b.M[0]);
        __m128 b1 = _mm_loadu_ps(b.M[1]);
        __m128 b2 = _mm_loadu_ps(b.M[2]);
        __m128 b3 = _mm_loadu_ps(b.M[3]);

        tmp = _mm_set_ps1(a.M[0][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(a.M[0][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(a.M[0][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(a.M[0][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[0], sum);

        tmp = _mm_set_ps1(a.M[1][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(a.M[1][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(a.M[1][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(a.M[1][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[1], sum);

        tmp = _mm_set_ps1(a.M[2][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(a.M[2][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(a.M[2][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(a.M[2][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[2], sum);

        tmp = _mm_set_ps1(a.M[3][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(a.M[3][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(a.M[3][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(a.M[3][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[3], sum);
#else
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m.M[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    m.M[i][j] += a.M[i][k] * b.M[k][j];
                }
            }
        }
#endif
        return m;
    }

    inline friend TVector operator*(const TMatrix &a, const TVector &b) {
        TVector4 vec(b, 1);
        vec = a * vec;
        return TVector(vec.X, vec.Y, vec.Z);
    }

    inline TMatrix &operator*=(const float &b) {
#if defined(USE_AVX_256)
        __m256 a0, b0;
        b0 = _mm256_set1_ps(b);
        a0 = _mm256_loadu_ps(M[0]);
        a0 = _mm256_mul_ps(a0, b0);
        _mm256_storeu_ps(M[0], a0);
        a0 = _mm256_loadu_ps(M[2]);
        a0 = _mm256_mul_ps(a0, b0);
        _mm256_storeu_ps(M[2], a0);
#elif defined(USE_AVX_128)
        __m128 a0, b0;
        b0 = _mm_set1_ps(b);
        a0 = _mm_loadu_ps(M[0]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[0], a0);
        a0 = _mm_loadu_ps(M[1]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[1], a0);
        a0 = _mm_loadu_ps(M[2]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[2], a0);
        a0 = _mm_loadu_ps(M[3]);
        a0 = _mm_add_ps(a0, b0);
        _mm_storeu_ps(M[3], a0);
#else
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                M[j][i] *= b;
#endif
        return *this;
    }

    inline TMatrix &operator*=(const TMatrix &b) {

        TMatrix m;
#if defined(USE_AVX_256) || defined(USE_AVX_128)
        __m128 tmp, sum;
        __m128 b0 = _mm_loadu_ps(b.M[0]);
        __m128 b1 = _mm_loadu_ps(b.M[1]);
        __m128 b2 = _mm_loadu_ps(b.M[2]);
        __m128 b3 = _mm_loadu_ps(b.M[3]);

        tmp = _mm_set_ps1(M[0][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(M[0][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(M[0][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(M[0][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[0], sum);

        tmp = _mm_set_ps1(M[1][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(M[1][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(M[1][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(M[1][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[1], sum);

        tmp = _mm_set_ps1(M[2][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(M[2][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(M[2][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(M[2][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[2], sum);

        tmp = _mm_set_ps1(M[3][0]);
        sum = _mm_mul_ps(tmp, b0);
        tmp = _mm_set_ps1(M[3][1]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b1), sum);
        tmp = _mm_set_ps1(M[3][2]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b2), sum);
        tmp = _mm_set_ps1(M[3][3]);
        sum = _mm_add_ps(_mm_mul_ps(tmp, b3), sum);
        _mm_storeu_ps(m.M[3], sum);
#else
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m.M[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    m.M[i][j] += M[i][k] * b.M[k][j];
                }
            }
        }
#endif
        return (*this = m);
    }


    // a == b
    inline friend bool operator==(const TMatrix &a, const TMatrix &b) {

        for (char j = 0; j < 4; j++)
            for (char i = 0; i < 4; i++)
                if (a.M[j][i] != b.M[j][i])
                    return false;
        return true;
    }

    inline friend bool operator!=(const TMatrix &a, const TMatrix &b) {
        return !(a == b);
    }

    inline friend bool operator%=(const TMatrix &a, const TMatrix &b) {
        for (char j = 0; j < 4; j++)
            for (char i = 0; i < 4; i++)
                if (!nearEq(a.M[j][i], b.M[j][i]))
                    return false;
        return true;
    }


    inline TMatrix operator~() {
        TMatrix m;
        m.M[0][0] = M[0][0];
        m.M[0][1] = M[1][0];
        m.M[0][2] = M[2][0];
        m.M[0][3] = M[3][0];
        m.M[1][0] = M[0][1];
        m.M[1][1] = M[1][1];
        m.M[1][2] = M[2][1];
        m.M[1][3] = M[3][1];
        m.M[2][0] = M[0][2];
        m.M[2][1] = M[1][2];
        m.M[2][2] = M[2][2];
        m.M[2][3] = M[3][2];
        m.M[3][0] = M[0][3];
        m.M[3][1] = M[1][3];
        m.M[3][2] = M[2][3];
        m.M[3][3] = M[3][3];
        return m;
    };

    inline TMatrix &Transpose() {
        std::swap(M[1][0], M[0][1]);
        std::swap(M[2][0], M[0][2]);
        std::swap(M[3][0], M[0][3]);
        std::swap(M[0][1], M[1][0]);
        std::swap(M[2][1], M[1][2]);
        std::swap(M[3][1], M[1][3]);
        std::swap(M[0][2], M[2][0]);
        std::swap(M[1][2], M[2][1]);
        std::swap(M[3][2], M[2][3]);
        std::swap(M[0][3], M[3][0]);
        std::swap(M[1][3], M[3][1]);
        std::swap(M[2][3], M[3][2]);
        return *this;
    }

    inline TMatrix &SetOrigin(const TVector &origin) {
        M[3][0] = origin.X;
        M[3][1] = origin.Y;
        M[3][2] = origin.Z;
        return *this;
    }

    [[maybe_unused]]
    inline TVector &GetOrigin() {
        return *((TVector *) (&M[3]));
    }

    inline TVector &operator[](int i) {
        assert(i >= 0 && i <= 3 && "TVector: Invalid axis");
        switch (i) {
            case 0:
                return *((TVector *) (&M[0]));
            case 1:
                return *((TVector *) (&M[1]));
            case 2:
                return *((TVector *) (&M[2]));
            case 3:
                return *((TVector *) (&M[3]));
            default:
                break;
        }
    }

    inline void GetAxes(TVector &AxisX, TVector &AxisY, TVector &AxisZ, TVector &AxisW) {
        AxisX = *((TVector *) (&M[0]));
        AxisY = *((TVector *) (&M[1]));
        AxisZ = *((TVector *) (&M[2]));
        AxisW = *((TVector *) (&M[3]));
    }

    inline void SetAxes(const TVector &AxisX, const TVector &AxisY, const TVector &AxisZ, const TVector &AxisW) {
        *((TVector *) (&M[0])) = AxisX;
        *((TVector *) (&M[1])) = AxisY;
        *((TVector *) (&M[2])) = AxisZ;
        *((TVector *) (&M[3])) = AxisW;
    }

    inline bool ContainsNaN() {
        for (auto &row: M)
            for (char i = 0; i < 4; i++)
                if (!finite(row[i]))
                    return false;
        return true;
    }


    inline void Stream(std::ostream &os) const {
        os << "TMatrix(" << std::endl
           << M[0][0] << ", " << M[0][1] << ", " << M[0][2] << ", " << M[0][3] << std::endl
           << M[1][0] << ", " << M[1][1] << ", " << M[1][2] << ", " << M[1][3] << std::endl
           << M[2][0] << ", " << M[2][1] << ", " << M[2][2] << ", " << M[2][3] << std::endl
           << M[3][0] << ", " << M[3][1] << ", " << M[3][2] << ", " << M[3][3] << ")";
    }
};