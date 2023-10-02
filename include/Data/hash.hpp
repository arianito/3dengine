#pragma once

#include <cstddef>
#include <cstdint>
#include "data/TString.hpp"
#include "engine/TVector.hpp"

#define XXH_INLINE_ALL
#include "xxhash.h"

#undef PERMUTE3
#define PERMUTE3(a, b, c) \
  do {                    \
    std::swap(a, b);      \
    std::swap(a, c);      \
  } while (0)


static const uint32_t c1 = 0xcc9e2d51;
static const uint32_t c2 = 0x1b873593;

inline static uint32_t Fetch32(const void *p) {
    uint32_t t;
    memcpy(&t, p, sizeof t);
    return t;
}

inline static uint32_t fmix(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

inline static uint32_t Rotate32(uint32_t val, int shift) {
    // Avoid shifting by 32: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
}

inline static uint32_t Mur(uint32_t a, uint32_t h) {
    // Helper from Murmur3 for combining two 32-bit values.
    a *= c1;
    a = Rotate32(a, 17);
    a *= c2;
    h ^= a;
    h = Rotate32(h, 19);
    return h * 5 + 0xe6546b64;
}

inline static uint32_t Hash32Len5to12(const char *s, size_t len) {
    uint32_t a = static_cast<uint32_t>(len), b = a * 5, c = 9, d = b;
    a += Fetch32(s);
    b += Fetch32(s + len - 4);
    c += Fetch32(s + ((len >> 1) & 4));
    return fmix(Mur(c, Mur(b, Mur(a, d))));
}

inline static uint32_t Hash32Len0to4(const char *s, size_t len) {
    uint32_t b = 0;
    uint32_t c = 9;
    for (size_t i = 0; i < len; i++) {
        signed char v = static_cast<signed char>(s[i]);
        b = b * c1 + static_cast<uint32_t>(v);
        c ^= b;
    }
    return fmix(Mur(b, Mur(static_cast<uint32_t>(len), c)));
}

inline static uint32_t Hash32Len13to24(const char *s, size_t len) {
    uint32_t a = Fetch32(s - 4 + (len >> 1));
    uint32_t b = Fetch32(s + 4);
    uint32_t c = Fetch32(s + len - 8);
    uint32_t d = Fetch32(s + (len >> 1));
    uint32_t e = Fetch32(s);
    uint32_t f = Fetch32(s + len - 4);
    uint32_t h = static_cast<uint32_t>(len);

    return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
}

inline static uint32_t gbswap_32(uint32_t host_int) {
    return (((host_int & uint32_t{0xFF}) << 24) |
            ((host_int & uint32_t{0xFF00}) << 8) |
            ((host_int & uint32_t{0xFF0000}) >> 8) |
            ((host_int & uint32_t{0xFF000000}) >> 24));
}

inline static uint32_t CityHash32(const char *s, size_t len) {
    if (len <= 24) {
        return len <= 12
               ? (len <= 4 ? Hash32Len0to4(s, len) : Hash32Len5to12(s, len))
               : Hash32Len13to24(s, len);
    }

    // len > 24
    uint32_t h = static_cast<uint32_t>(len), g = c1 * h, f = g;

    uint32_t a0 = Rotate32(Fetch32(s + len - 4) * c1, 17) * c2;
    uint32_t a1 = Rotate32(Fetch32(s + len - 8) * c1, 17) * c2;
    uint32_t a2 = Rotate32(Fetch32(s + len - 16) * c1, 17) * c2;
    uint32_t a3 = Rotate32(Fetch32(s + len - 12) * c1, 17) * c2;
    uint32_t a4 = Rotate32(Fetch32(s + len - 20) * c1, 17) * c2;
    h ^= a0;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    h ^= a2;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a1;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    g ^= a3;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    f += a4;
    f = Rotate32(f, 19);
    f = f * 5 + 0xe6546b64;
    size_t iters = (len - 1) / 20;
    do {
        uint32_t b0 = Rotate32(Fetch32(s) * c1, 17) * c2;
        uint32_t b1 = Fetch32(s + 4);
        uint32_t b2 = Rotate32(Fetch32(s + 8) * c1, 17) * c2;
        uint32_t b3 = Rotate32(Fetch32(s + 12) * c1, 17) * c2;
        uint32_t b4 = Fetch32(s + 16);
        h ^= b0;
        h = Rotate32(h, 18);
        h = h * 5 + 0xe6546b64;
        f += b1;
        f = Rotate32(f, 19);
        f = f * c1;
        g += b2;
        g = Rotate32(g, 18);
        g = g * 5 + 0xe6546b64;
        h ^= b3 + b1;
        h = Rotate32(h, 19);
        h = h * 5 + 0xe6546b64;
        g ^= b4;
        g = gbswap_32(g) * 5;
        h += b4 * 5;
        h = gbswap_32(h);
        f += b0;
        PERMUTE3(f, h, g);
        s += 20;
    } while (--iters != 0);
    g = Rotate32(g, 11) * c1;
    g = Rotate32(g, 17) * c1;
    f = Rotate32(f, 11) * c1;
    f = Rotate32(f, 17) * c1;
    h = Rotate32(h + g, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    h = Rotate32(h + f, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    return h;
}

#define big_prime_int32 (4247846927ul)

#define general_hash_function(input, len, seed) (XXH32(input, len, seed) % big_prime_int32)


template<typename T>
inline static uint32_t hash_type(const T &key, uint32_t seed);

template<>
inline uint32_t hash_type<TStringView>(const TStringView &key, uint32_t seed) {
    return general_hash_function(key.begin(), key.length(), seed);
}

template<>
inline uint32_t hash_type<char>(const char &key, uint32_t seed) {
    union {
        short value;
        char bytes[1];
    } converter{key};
    return general_hash_function(converter.bytes, 1, seed);
}

template<>
inline uint32_t hash_type<short>(const short &key, uint32_t seed) {
    union {
        short value;
        char bytes[2];
    } converter{key};
    return general_hash_function(converter.bytes, 2, seed);
}

template<>
inline uint32_t hash_type<int>(const int &key, uint32_t seed) {
    union {
        int value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}

template<>
inline uint32_t hash_type<unsigned int>(const unsigned int &key, uint32_t seed) {
    union {
        unsigned int value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}

template<>
inline uint32_t hash_type<unsigned long long>(const unsigned long long &key, uint32_t seed) {
    union {
        unsigned long long value;
        char bytes[8];
    } converter{key};
    return general_hash_function(converter.bytes, 8, seed);
}

template<>
inline uint32_t hash_type<float>(const float &key, uint32_t seed) {
    union {
        float value;
        char bytes[4];
    } converter{key};
    return general_hash_function(converter.bytes, 4, seed);
}


template<>
inline uint32_t hash_type<Vec2>(const Vec2 &key, uint32_t seed) {
    union {
        Vec2 value;
        char bytes[8];
    } converter{key};
    return general_hash_function(converter.bytes, 8, seed);
}

template<>
inline uint32_t hash_type<Vec3>(const Vec3 &key, uint32_t seed) {
    union {
        Vec3 value;
        char bytes[12];
    } converter{key};
    return general_hash_function(converter.bytes, 12, seed);
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
    return general_hash_function(converter.bytes, 32, seed);
}