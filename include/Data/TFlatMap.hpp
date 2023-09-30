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

template<typename K, typename V, class TAlloc = FreeListMemory>
class TFlatMap {
private:
    struct __attribute__((aligned(32))) Node {
        K key;
        V value;
        bool used;
    };
private:
    Node *mProbes{nullptr};
    uint32_t mCapacity{2};
    uint32_t mLength{0};
    bool mAllowExpansion{true};

public:
    explicit inline TFlatMap() : TFlatMap(2, true) {}

    explicit inline TFlatMap(uint32_t capacity) : TFlatMap(capacity, true) {}

    explicit inline TFlatMap(uint32_t capacity, bool expansion) : mAllowExpansion(expansion) {
        assert(!(capacity & (capacity - 1)) && "TFlatMap: Capacity should be power of 2");
        mCapacity = expansion ? (capacity) : (capacity << 1);
        mProbes = Alloc<TAlloc, Node, true>(mCapacity);
    }

    explicit inline TFlatMap(const TFlatMap &) = delete;

    inline ~TFlatMap() {
        Free<TAlloc>(&mProbes);
    }

    inline void Reserve(uint32_t newCapacity) {
        if (newCapacity < (mLength << 1))
            return;
        Node *newList = Alloc<TAlloc, Node, true>(newCapacity);
        for (uint32_t i = 0; i < mCapacity; i++) {
            if (mProbes[i].used) {
                Node *probe = &mProbes[i];
                uint32_t index;
                linearProbeSet(newList, probe->key, &index, newCapacity);
                Node &newNode = newList[index];
                newNode.key = probe->key;
                newNode.value = probe->value;
                newNode.used = probe->used;
            }
        }

        Free<TAlloc>(&mProbes);
        mProbes = newList;
        mCapacity = newCapacity;
    }

    inline void Fit() {
        Reserve(NEXTPOW2(mLength));
    }

    inline bool Set(const K &key, const V &value) {
        if (mAllowExpansion) expand();
        uint32_t index;
        if (!linearProbeSet(mProbes, key, &index, mCapacity)) return false;
        auto &node = mProbes[index];
        if (!mAllowExpansion && ((mLength << 1) > mCapacity - 1) && !node.used) return false;
        if (!node.used) mLength++;
        node.key = key;
        node.value = value;
        node.used = true;
        return true;
    }

    inline const V &Get(const K &key) const {
        uint32_t index;
        bool found = linearProbeGet(key, &index, mCapacity);
        assert(found && "TFlatMap: not found");
        Node *probe = &mProbes[index];
        return probe->value;
    }

    inline const V &operator[](const K &key) const {
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
        return mAllowExpansion ? mCapacity : (mCapacity >> 1);
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
        if (mCapacity > mLength) return;
        Reserve(mCapacity << 1);
    }

    inline bool linearProbeSet(const Node *mList, const K &key, uint32_t *foundIndex, uint32_t capacity) const {
        uint32_t hash = hash_type<K>(key, 0) & (capacity - 1);
        const Node *node = &mList[hash];
        if (!node->used || node->key == key) {
            *foundIndex = hash;
            return true;
        }
        hash = hash_type(key, 1);
        uint32_t seed = 0;
        do {
            uint32_t index = (hash + seed++) & (capacity - 1);
            node = &mList[index];
            if (!node->used || node->key == key) {
                *foundIndex = index;
                return true;
            }
        } while (seed < capacity);
        return false;
    }

    inline bool linearProbeGet(const K &key, uint32_t *foundIndex, uint32_t capacity) const {
        uint32_t hash = hash_type<K>(key, 0) & (capacity - 1);
        Node *node = &mProbes[hash];
        if (node->used && node->key == key) {
            *foundIndex = hash;
            return true;
        } else if (!node->used) return false;
        hash = hash_type(key, 1);
        uint32_t seed = 0;
        do {
            uint32_t index = (hash + seed++) & (capacity - 1);
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