#pragma once

#include <cassert>
#include <cstring>
#include <utility>

extern "C" {
#include "mem/utils.h"
}

#include "data/hash.hpp"
#include "data/TString.hpp"
#include "engine/Memory.hpp"

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

template<typename K, typename V, class TAlloc = FreeListMemory>
class TFlatMap {
private:
    struct __attribute__((aligned(32))) Node {
        K key;
        V value;
        bool used;
    };
public:
    Node *mProbes{nullptr};
    uint32_t mCapacity{2};
    uint32_t mLength{0};
    uint32_t mNextPrime{0};
    uint32_t mPeak{0};
    uint32_t mSeed{0};
    static constexpr double mMaxLoadFactor{0.5f};

public:
    explicit inline TFlatMap() : TFlatMap(2) {}

    explicit inline TFlatMap(uint32_t capacity) : mCapacity(capacity) {
        assert(!(capacity & (capacity - 1)) && "TFlatMap: Capacity should be power of 2");
        Reserve(mCapacity);
    }

    explicit inline TFlatMap(const TFlatMap &) = delete;

    inline ~TFlatMap() {
        Free<TAlloc>(&mProbes);
    }

    inline void Reserve(uint32_t newCapacity) {
        if (newCapacity < (mLength << 1))
            return;
        Node *newList = Alloc<TAlloc, Node, true>(newCapacity);
        if (mProbes != nullptr) {
            for (uint32_t i = 0; i < mCapacity; i++) {
                if (mProbes[i].used) {
                    Node *probe = &mProbes[i];
                    uint32_t index;
                    index = linearProbeSet(newList, probe->key, newCapacity);
                    Node &newNode = newList[index];
                    newNode.key = probe->key;
                    newNode.value = probe->value;
                    newNode.used = probe->used;
                }
            }
            Free<TAlloc>(&mProbes);
        }
        for (int i = 1; i < 40; i++) {
            if (newCapacity < primes[i]) {
                mNextPrime = primes[i - 1];
                break;
            }
        }
        mProbes = newList;
        mCapacity = newCapacity;
        printf("nextPrime: %d, mCapa: %d\n", mNextPrime, mCapacity);
    }

    inline void Fit() {
        Reserve(NEXTPOW2(mLength));
    }

    inline bool Set(const K &key, const V &value) {
        expand();
        uint32_t index = linearProbeSet(mProbes, key, mCapacity);
        auto &node = mProbes[index];
        if (!node.used) mLength++;
        node.key = key;
        node.value = value;
        node.used = true;
        return true;
    }

    inline const V &Get(const K &key) {
        uint32_t index;
        bool found = linearProbeGet(key, &index, mCapacity);
        assert(found && "TFlatMap: not found");
        Node *probe = &mProbes[index];
        return probe->value;
    }

    inline const V &operator[](const K &key) {
        return Get(key);
    }


    [[maybe_unused]] inline void Remove(const K &key) {
        uint32_t index;
        if (!linearProbeGet(key, &index, mCapacity)) return;
        auto &probe = mProbes[index];
        probe.used = false;
        mLength--;
    }

    inline bool Contains(const K &key) {
        uint32_t index;
        return linearProbeGet(key, &index, mCapacity);
    }

    [[maybe_unused]] [[nodiscard]]
    inline uint32_t Capacity() {
        return mCapacity;
    }

    [[maybe_unused]] [[nodiscard]]
    inline uint32_t Length() {
        return mLength;
    }

    [[maybe_unused]] inline void Clear() {
        unsigned int nBytes = mCapacity * sizeof(Node);
        memset(mProbes, 0, nBytes);
        mLength = 0;
    }


private:
    inline void expand() {
        if (mCapacity > (mLength << 1)) return;
        Reserve(mCapacity << 1);
    }


    inline uint32_t linearProbeSet(Node *mList, const K &key, uint32_t capacity) {
        uint32_t seed = 0;
        uint32_t hash = hash_type<K>(key, 0);
        do {
            uint32_t index = (hash + seed++) % mNextPrime;
            Node *node = &mList[index];
            if (!node->used || node->key == key) {
                if (seed > mPeak)
                    mPeak = seed;
                return index;
            }
        } while (true);
    }

    inline bool linearProbeGet(const K &key, uint32_t *foundIndex, uint32_t capacity) {
        uint32_t hash = hash_type<K>(key, 0) % mNextPrime;
        Node *node = &mProbes[hash];
        if (node->used && node->key == key) {
            *foundIndex = hash;
            return true;
        } else if (!node->used) return false;
        uint32_t seed = 1;
        do {
            uint32_t index = (hash + seed++) % mNextPrime;
            node = &mProbes[index];
            if (node->used && node->key == key) {
                *foundIndex = index;
                return true;
            } else if (!node->used) return false;
        } while (seed < mCapacity);
        return false;
    }

public:
    class Iterator {
    private:
        Node *mCurrent;
        Node *mEnd;

    public:
        explicit inline Iterator(Node *current, Node *end) : mCurrent(current), mEnd(end) {
            while (mCurrent != mEnd && !mCurrent->used) {
                ++mCurrent;
            }
        }

        inline Iterator &operator++() {
            ++mCurrent;
            while (mCurrent != mEnd && !mCurrent->used) {
                ++mCurrent;
            }
            return *this;
        }

        inline bool operator!=(const Iterator &other) const {
            return mCurrent != other.mEnd;
        }

        inline std::pair<K &, V &> operator*() const {
            return {mCurrent->key, mCurrent->value};
        }
    };

    Iterator begin() {
        return Iterator(mProbes, mProbes + mCapacity);
    }

    Iterator end() {
        return Iterator(mProbes + mCapacity, mProbes + mCapacity);
    }
};

