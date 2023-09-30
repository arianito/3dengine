#pragma once

#include <cstddef>
#include <cstdint>
#include "data/TString.hpp"
#include "engine/TVector.hpp"

#define XXH_INLINE_ALL

#include "xxhash.h"

template<typename T>
inline static uint32_t hash_type(const T &key, uint32_t seed);

template<>
inline uint32_t hash_type<TStringView>(const TStringView &key, uint32_t seed) {
    return XXH32(key.begin(), key.length(), seed);
}

template<>
inline uint32_t hash_type<char>(const char &key, uint32_t seed) {
    union {
        short value;
        char bytes[1];
    } converter{key};
    return XXH32(converter.bytes, 1, seed);
}

template<>
inline uint32_t hash_type<short>(const short &key, uint32_t seed) {
    union {
        short value;
        char bytes[2];
    } converter{key};
    return XXH32(converter.bytes, 2, seed);
}

template<>
inline uint32_t hash_type<int>(const int &key, uint32_t seed) {
    union {
        int value;
        char bytes[4];
    } converter{key};
    return XXH32(converter.bytes, 4, seed);
}

template<>
inline uint32_t hash_type<unsigned int>(const unsigned int &key, uint32_t seed) {
    union {
        unsigned int value;
        char bytes[4];
    } converter{key};
    return XXH32(converter.bytes, 4, seed);
}

template<>
inline uint32_t hash_type<unsigned long long>(const unsigned long long &key, uint32_t seed) {
    union {
        unsigned long long value;
        char bytes[8];
    } converter{key};
    return XXH32(converter.bytes, 8, seed);
}

template<>
inline uint32_t hash_type<float>(const float &key, uint32_t seed) {
    union {
        float value;
        char bytes[4];
    } converter{key};
    return XXH32(converter.bytes, 4, seed);
}


template<>
inline uint32_t hash_type<Vec2>(const Vec2 &key, uint32_t seed) {
    union {
        Vec2 value;
        char bytes[8];
    } converter{key};
    return XXH32(converter.bytes, 8, seed);
}

template<>
inline uint32_t hash_type<Vec3>(const Vec3 &key, uint32_t seed) {
    union {
        Vec3 value;
        char bytes[12];
    } converter{key};
    return XXH32(converter.bytes, 12, seed);
}

template<>
inline uint32_t hash_type<Edge>(const Edge &key, uint32_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed);
}

template<>
inline uint32_t hash_type<Triangle>(const Triangle &key, uint32_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed) ^
           hash_type(key.c, seed);
}

template<>
inline uint32_t hash_type<Tetrahedron>(const Tetrahedron &key, uint32_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed) ^
           hash_type(key.c, seed) ^
           hash_type(key.d, seed);
}

template<>
inline uint32_t hash_type<BBox>(const BBox &key, uint32_t seed) {
    union {
        BBox value;
        float items[8];
        char bytes[32];
    } converter{key};
    converter.items[3] = 0;
    converter.items[7] = 0;
    return XXH32(converter.bytes, 32, seed);
}