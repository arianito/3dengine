#pragma once

#include <cstddef>
#include <cstdint>
#include "data/TString.hpp"
#include "engine/TVector.hpp"

#define XXH_INLINE_ALL
#include "xxhash.h"

#define general_hash_function(input, len, seed) (XXH64(input, len, seed))


template<typename T>
inline static uint64_t hash_type(const T &key, uint64_t seed);

template<>
inline uint64_t hash_type<TStringView>(const TStringView &key, uint64_t seed) {
    return general_hash_function(key.begin(), key.length(), seed);
}

template<>
inline uint64_t hash_type<char>(const char &key, uint64_t seed) {
    union {
        short value;
        char bytes[1];
    } converter{key};
    return general_hash_function(converter.bytes, 1, seed);
}

template<>
inline uint64_t hash_type<short>(const short &key, uint64_t seed) {
    union {
        short value;
        char bytes[2];
    } converter{key};
    return general_hash_function(converter.bytes, 2, seed);
}

template<>
inline uint64_t hash_type<int>(const int &key, uint64_t seed) {
    union {
        int value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}

template<>
inline uint64_t hash_type<unsigned int>(const unsigned int &key, uint64_t seed) {
    union {
        unsigned int value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}

template<>
inline uint64_t hash_type<unsigned long long>(const unsigned long long &key, uint64_t seed) {
    union {
        unsigned long long value;
        char bytes[8];
    } converter{key};
    return general_hash_function(converter.bytes, 8, seed);
}

template<>
inline uint64_t hash_type<float>(const float &key, uint64_t seed) {
    union {
        float value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}


template<>
inline uint64_t hash_type<Vec2>(const Vec2 &key, uint64_t seed) {
    union {
        Vec2 value;
        char bytes[8];
    } converter{key};
    return general_hash_function(converter.bytes, 8, seed);
}

template<>
inline uint64_t hash_type<Vec3>(const Vec3 &key, uint64_t seed) {
    union {
        Vec3 value;
        float floats[4];
        char bytes[16];
    } converter{key};
    converter.floats[3] = 0;
    return general_hash_function(converter.bytes, 16, seed);
}

template<>
inline uint64_t hash_type<Edge>(const Edge &key, uint64_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed);
}

template<>
inline uint64_t hash_type<Triangle>(const Triangle &key, uint64_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed) ^
           hash_type(key.c, seed);
}

template<>
inline uint64_t hash_type<Tetrahedron>(const Tetrahedron &key, uint64_t seed) {
    return hash_type(key.a, seed) ^
           hash_type(key.b, seed) ^
           hash_type(key.c, seed) ^
           hash_type(key.d, seed);
}

template<>
inline uint64_t hash_type<BBox>(const BBox &key, uint64_t seed) {
    union {
        BBox value;
        float items[8];
        char bytes[32];
    } converter{key};
    converter.items[3] = 0;
    converter.items[7] = 0;
    return general_hash_function(converter.bytes, 32, seed);
}