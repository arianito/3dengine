#pragma once

#include "mathf.h"

constexpr unsigned int noise_1d(const int &PositionX, const unsigned int &Seed = 0) {
    constexpr unsigned int SQ5_Bit_Noise1 = 0xd2a80a3f;
    constexpr unsigned int SQ5_Bit_Noise2 = 0xa884f197;
    constexpr unsigned int SQ5_Bit_Noise3 = 0x6C736F4B;
    constexpr unsigned int SQ5_Bit_Noise4 = 0xB79F3ABB;
    constexpr unsigned int SQ5_Bit_Noise5 = 0x1b56c4f5;
    unsigned int MangledBits = static_cast<unsigned int>(PositionX);
    MangledBits *= SQ5_Bit_Noise1;
    MangledBits += Seed;
    MangledBits ^= (MangledBits >> 9);
    MangledBits += SQ5_Bit_Noise2;
    MangledBits ^= (MangledBits >> 11);
    MangledBits *= SQ5_Bit_Noise3;
    MangledBits ^= (MangledBits >> 13);
    MangledBits += SQ5_Bit_Noise4;
    MangledBits ^= (MangledBits >> 15);
    MangledBits *= SQ5_Bit_Noise5;
    MangledBits ^= (MangledBits >> 17);
    return MangledBits;
}

constexpr unsigned int noise_2d(const int &PositionX,
                          const int &PositionY,
                          const unsigned int &Seed = 0) {
    constexpr unsigned int Large_Prime = 198491317;
    return noise_1d(PositionX + (Large_Prime * PositionY), Seed);
}

constexpr unsigned int noise_3d(const int &PositionX,
                          const int &PositionY,
                          const int &PositionZ,
                          const unsigned int &Seed = 0) {
    constexpr unsigned int Large_Prime_1 = 198491317;
    constexpr unsigned int Large_Prime_2 = 6542989;
    return noise_1d(
            PositionX +
            (Large_Prime_1 * PositionY) +
            (Large_Prime_2 * PositionZ), Seed);
}

constexpr unsigned int noise_4d(const int &PositionX,
                          const int &PositionY,
                          const int &PositionZ,
                          const int &PositionW,
                          const unsigned int &Seed = 0) {
    constexpr unsigned int Large_Prime_1 = 198491317;
    constexpr unsigned int Large_Prime_2 = 6542989;
    constexpr unsigned int Large_Prime_3 = 357239;
    return noise_1d(
            PositionX +
            (Large_Prime_1 * PositionY) +
            (Large_Prime_2 * PositionZ) +
            (Large_Prime_3 * PositionW), Seed);
}

constexpr float noise_1d_zero_to_one(const int &PositionX,
                                  const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0xFFFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_1d(PositionX, Seed));
}

constexpr float noise_2d_zero_to_one(const int &PositionX,
                                  const int &PositionY,
                                  const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0xFFFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_2d(PositionX, PositionY, Seed));
}

constexpr float noise_3d_zero_to_one(const int &PositionX,
                                  const int &PositionY,
                                  const int &PositionZ,
                                  const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0xFFFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_3d(PositionX, PositionY, PositionZ, Seed));
}

constexpr float noise_4d_zero_to_one(const int &PositionX,
                                  const int &PositionY,
                                  const int &PositionZ,
                                  const int &PositionW,
                                  const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0xFFFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_4d(PositionX, PositionY, PositionZ, PositionW, Seed));
}


constexpr float noise_1d_neg_one_to_one(const int &PositionX,
                                    const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0x7FFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_1d(PositionX, Seed));
}

constexpr float noise_2d_neg_one_to_one(const int &PositionX,
                                    const int &PositionY,
                                    const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0x7FFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_2d(PositionX, PositionY, Seed));
}

constexpr float noise_3d_neg_one_to_one(const int &PositionX,
                                    const int &PositionY,
                                    const int &PositionZ,
                                    const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0x7FFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_3d(PositionX, PositionY, PositionZ, Seed));
}

constexpr float noise_4d_neg_one_to_one(const int &PositionX,
                                    const int &PositionY,
                                    const int &PositionZ,
                                    const int &PositionW,
                                    const unsigned int &Seed = 0) {
    constexpr double One_Over_Max_Uint = (1.0 / static_cast<double>(0x7FFFFFFF));
    return One_Over_Max_Uint * static_cast<double>(noise_4d(PositionX, PositionY, PositionZ, PositionW, Seed));
}

constexpr float dot_grad_1d(
        const int &X,
        const float &X0,
        const unsigned int &Seed
) {
    const float T = noise_1d_neg_one_to_one(X, Seed);
    return ((X0 - X) * T);
}

constexpr float dot_grad_2d(
        const int &X,
        const int &Y,
        const float &X0,
        const float &Y0,
        const unsigned int &Seed
) {
    const float T = noise_2d_neg_one_to_one(X, Y, Seed) * PI * 2;
    return ((X0 - X) * cosf(T)) + ((Y0 - Y) * sinf(T));
}

constexpr float fade(const float &T) {
    return ((6 * T - 15) * T + 10) * T * T * T;
}

static inline float noise_perlin_1d(const float &X, const float &Scale, const unsigned int &Seed) {
    const float ScaleClamped = Scale <= 0 ? 1.0f : Scale;
    const float Xs = X / ScaleClamped;
    const int X0 = (int)floorf(Xs);
    const int X1 = X0 + 1;
    const float Dx = fade(Xs - X0);
    return lerp(
            dot_grad_1d(X0, Xs, Seed),
            dot_grad_1d(X1, Xs, Seed), Dx);
}

static inline float noise_perlin_2d(const float &X, const float &Y, const float &Scale, const unsigned int &Seed) {
    const float ScaleClamped = Scale <= 0 ? 1.0f : Scale;
    const float Xs = X / ScaleClamped;
    const float Ys = Y / ScaleClamped;
    const int X0 = (int)floorf(Xs);
    const int X1 = X0 + 1;
    const int Y0 = (int)floorf(Ys);
    const int Y1 = Y0 + 1;
    const float Dx = fade(Xs - X0);
    const float Dy = fade(Ys - Y0);
    return lerp(
            lerp(
                    dot_grad_2d(X0, Y0, Xs, Ys, Seed),
                    dot_grad_2d(X1, Y0, Xs, Ys, Seed), Dx),
            lerp(
                    dot_grad_2d(X0, Y1, Xs, Ys, Seed),
                    dot_grad_2d(X1, Y1, Xs, Ys, Seed), Dx), Dy);
}
