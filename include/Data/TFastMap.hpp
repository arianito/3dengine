#pragma once


#include <immintrin.h>
#include <cstdint>
#include "data/hash.hpp"

#define H1(hash) (hash >> 7)
#define H2(hash) (hash & 0x7F)

#define IS_EMPTY (c) (c == kEmpty)
#define IS_FULL (c) ((int8_t)c >= 0)
#define IS_DELETED (c) (c == kDeleted)
#define IS_EMPTY_OR_DELETED (c) ((int8_t)c < kSentinel)

template<typename TKey, typename TValue, typename TAlloc = FreeListMemory>
class TFastMap {
private:
    enum Ctrl : int8_t {
        kEmpty = -128, // 0b10000000
        kDeleted = -2, // 0b11111110
        kSentinel = -1,// 0b11111111
    };
    static constexpr unsigned int primes[90] = {
            2ul, 3ul, 5ul, 7ul, 11ul, 13ul, 17ul, 23ul, 29ul, 37ul, 47ul,
            59ul, 73ul, 97ul, 127ul, 151ul, 197ul, 251ul, 313ul, 397ul,
            499ul, 631ul, 797ul, 1009ul, 1259ul, 1597ul, 2011ul, 2539ul,
            3203ul, 4027ul, 5087ul, 6421ul, 8089ul, 10193ul, 12853ul, 16193ul,
            20399ul, 25717ul, 32401ul, 40823ul, 51437ul, 64811ul, 81649ul,
            102877ul, 129607ul, 163307ul, 205759ul, 259229ul, 326617ul,
            411527ul, 518509ul, 653267ul, 823117ul, 1037059ul, 1306601ul,
            1646237ul, 2074129ul, 2613229ul, 3292489ul, 4148279ul, 5226491ul,
            6584983ul, 8296553ul, 10453007ul, 13169977ul, 16593127ul, 20906033ul,
            26339969ul, 33186281ul, 41812097ul, 52679969ul, 66372617ul,
            83624237ul, 105359939ul, 132745199ul, 167248483ul, 210719881ul,
            265490441ul, 334496971ul, 421439783ul, 530980861ul, 668993977ul,
            842879579ul, 1061961721ul, 1337987929ul, 1685759167ul, 2123923447ul,
            2675975881ul, 3371518343ul, 4247846927ul
    };

    struct Node {
        TKey key;
        TValue value;
    };
    struct Group {
        __m128i control;
        Node nodes[16];
    };
private:
    class iterator {
    private:
        Group *mBegin{};
        Group *mEnd{};
        Node *mCurrent{};
        uint32_t mIndex = 0;
    public:
        explicit inline iterator() = default;

        explicit inline iterator(Group *begin, Group *end) : mBegin(begin), mEnd(end) {
            mIndex = 0;
            mCurrent = next();
        }


        inline iterator &operator++() {
            mCurrent = next();
            return *this;
        }

        inline bool operator!=(const iterator &other) const {
            return mBegin != other.mEnd;
        }

        inline Node *operator*() const {
            return mCurrent;
        }

    private:
        Node *next() {
            while (mBegin != mEnd) {
                uint16_t matches = _mm_movemask_epi8(mBegin->control);
                matches >>= mIndex;
                while (mIndex < 16) {
                    if (!(matches & 1))
                        return &mBegin->nodes[mIndex++];
                    matches >>= 1;
                    mIndex++;
                }
                mBegin++;
                mIndex = 0;
            }
            return nullptr;
        }
    };

private:
    Group *mGroups{};
    uint32_t mLength{};
    uint32_t mNumGroups{};
    uint32_t mPrimeIdx{0};
    double mLoadFactor{0};

public:
    explicit inline TFastMap() {
        reserve(2);
    }

    explicit inline TFastMap(const TFastMap &) = delete;

    inline ~TFastMap() {
        Free<TAlloc>(&mGroups);
    }

    inline void Set(const TKey &key, const TValue &value) {
        Node *node = set(key);
        if (!node) return;
        node->value = value;
    }

    inline TValue *Get(const TKey &key) {
        Node *node = get(key);
        if (!node) return nullptr;
        return &(node->value);
    }

    inline TValue *operator[](const TKey &key) {
        return Get(key);
    }

    inline bool Contains(const TKey &key) {
        return get(key) != nullptr;
    }

    inline bool Remove(const TKey &key) {
        return remove(key);
    }

    inline void Fit() {
        for (int i = 0; i < 90; i++) {
            if (primes[i] > mLength) {
                mPrimeIdx = i;
                rehash(primes[mPrimeIdx++]);
                return;
            }
        }
    }

    inline uint32_t Capacity() {
        return mNumGroups * 16;
    }

    inline uint32_t Length() {
        return mLength;
    }

    inline iterator begin() {
        return iterator(mGroups, mGroups + mNumGroups);
    }

    inline iterator end() {
        return iterator(mGroups + mNumGroups, mGroups + mNumGroups);
    }

private:
    inline Node *set(const TKey &key) {
        if (mLoadFactor >= 0.5)
            rehash(primes[mPrimeIdx++]);
        const uint32_t hash = hash_type<TKey>(key, 232);
        uint32_t groupIndex{H1(hash) % mNumGroups};
        uint8_t h2 = H2(hash);
        Group *g = &mGroups[groupIndex];
        uint16_t matches = match(g->control, h2);
        int8_t i = 0;
        while (matches) {
            if ((matches & 1) && g->nodes[i].key == key)
                return &g->nodes[i];
            matches >>= 1;
            i++;
        }
        if (!matchEmpty(g->control)) {
            rehash(primes[mPrimeIdx++]);
            printf("rehash\n");
            return set(key);
        }

        int8_t freeIndex = 0;
        matches = _mm_movemask_epi8(g->control);
        i = 0;
        while (matches) {
            if (matches & 1) {
                freeIndex = i;
                break;
            }
            matches >>= 1;
            i++;
        }

        auto simdArray = reinterpret_cast<uint8_t *>(&g->control);
        simdArray[freeIndex] = h2;
        g->nodes[freeIndex].key = key;
        mLength++;
        mLoadFactor = ((double) mLength / (mNumGroups << 4));
        return &g->nodes[freeIndex];
    }

    inline Node *get(const TKey &key) {
        const uint32_t hash = hash_type<TKey>(key, 232);
        uint32_t groupIndex{H1(hash) % mNumGroups};
        uint8_t h2 = H2(hash);
        Group *g;
        while (true) {
            g = &mGroups[groupIndex];
            uint16_t matches = match(g->control, h2);
            int i = 0;
            while (matches) {
                if ((matches & 1) && g->nodes[i].key == key)
                    return &g->nodes[i];
                matches >>= 1;
                i++;
            }
            if (matchEmpty(g->control)) break;
            groupIndex = (groupIndex + 1) % mNumGroups;
        }
        return nullptr;
    }

    inline bool remove(const TKey &key) {
        const uint32_t hash = hash_type<TKey>(key, 232);
        uint32_t groupIndex{H1(hash) % mNumGroups};
        uint8_t h2 = H2(hash);
        Group *g;
        while (true) {
            g = &mGroups[groupIndex];
            uint16_t matches = match(g->control, h2);
            int i = 0;
            while (matches) {
                if ((matches & 1) && g->nodes[i].key == key) {
                    auto simdArray = reinterpret_cast<uint8_t *>(&g->control);
                    simdArray[i] = kDeleted;
                    mLength--;
                    return true;
                }
                matches >>= 1;
                i++;
            }
            if (matchEmpty(g->control)) break;
            groupIndex = (groupIndex + 1) % mNumGroups;
        }
        return false;
    }

    inline void reserve(const uint32_t &numGroups) {
        mGroups = Alloc<TAlloc, Group, true>(numGroups);
        for (int i = 0; i < numGroups; i++) mGroups[i].control = _mm_set1_epi8(kEmpty);
        mNumGroups = numGroups;
    }

    inline void rehash(uint32_t newSize) {
        Group *oldGroups = mGroups;
        uint32_t nNumOldGroups = mNumGroups;
        reserve(newSize);
        iterator it{oldGroups, oldGroups + nNumOldGroups};
        iterator end{oldGroups + nNumOldGroups, oldGroups + nNumOldGroups};
        mLength = 0;
        for (; it != end; ++it) {
            const auto &node = (*it);
            Set(node->key, node->value);
        }
        Free<TAlloc>(&oldGroups);
    }

    inline static uint16_t matchEmpty(__m128i ctrl) {
        return (uint16_t) _mm_movemask_epi8(ctrl);
//        return (uint16_t) _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_set1_epi8(kEmpty), ctrl));
    }

    inline static uint16_t match(__m128i ctrl, int8_t hash) {
        return (uint16_t) _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_set1_epi8(hash), ctrl));
    }
};