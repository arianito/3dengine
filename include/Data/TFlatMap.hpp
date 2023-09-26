#pragma once

#include <cassert>
#include <cstring>
#include <utility>

extern "C" {
#include "mem/utils.h"
}

#include "data/TString.hpp"
#include "engine/Memory.hpp"

template<typename K, typename V, class TAlloc = FreeListMemory>
class TFlatMap {
private:
    struct Node {
        K key;
        V value;
        bool used;
    };

public:
    explicit inline TFlatMap() : TFlatMap(8) {}

    explicit inline TFlatMap(int capacity) : mCapacity(capacity) {
        mProbes = Alloc<TAlloc, Node, true>(mCapacity);
    }

    explicit inline TFlatMap(const TFlatMap &) = delete;

    inline ~TFlatMap() {
        Free<TAlloc>(&mProbes);
    }

    inline void Reserve(int newCapacity) {
        if (newCapacity < (mLength << 1))
            return;

        Node *newList = Alloc<TAlloc, Node, true>(newCapacity);

        for (int i = 0; i < mCapacity; i++) {
            if (mProbes[i].used) {
                Node &probe = mProbes[i];
                unsigned int index = hash(probe.key, newCapacity);
                while (newList[index].used && newList[index].key != probe.key) {
                    index = (index + 1) % newCapacity;
                }
                Node &newNode = newList[index];
                newNode.key = probe.key;
                newNode.value = probe.value;
                newNode.used = probe.used;
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
        expand();
        int index = linearProbeSet(key);
        if (index < 0)
            return false;

        auto &node = mProbes[index];

        if (!node.used)
            mLength++;

        node.key = key;
        node.value = value;
        node.used = true;
        return true;
    }

    [[maybe_unused]] inline void Remove(const K &key) {
        int index = linearProbeGet(key);
        assert(index >= 0 && "ProbeHashTable: key not found");
        auto &probe = mProbes[index];
        if (probe.used && probe.key == key) {
            probe.used = false;
            mLength--;
        }
    }

    inline bool Contains(const K &key) {
        int index = linearProbeGet(key);
        Node &probe = mProbes[index];
        return index >= 0 && probe.used;
    }

    inline V &operator[](const K &key) {
        int index = linearProbeGet(key);
        Node &probe = mProbes[index];
        assert((index >= 0 && probe.used) && "ProbeHashTable: not found");
        return probe.value;
    }

    [[maybe_unused]] [[nodiscard]]
    inline const int &Capacity() const {
        return mCapacity;
    }

    [[maybe_unused]] [[nodiscard]]
    inline const int &Length() const {
        return mLength;
    }

    [[maybe_unused]] inline void Clear() {
        unsigned int nBytes = mCapacity * sizeof(Node);
        memset(mProbes, 0, nBytes);
        mLength = 0;
    }

private:
    static constexpr size_t mPrimeA{492366587};
    static constexpr size_t mPrimeB{1645333507};
    static constexpr size_t mPrimeC{6692367337};
    Node *mProbes{nullptr};
    int mCapacity{};
    int mLength{0};

private:
    inline void expand() {
        float ratio = (float) mLength / mCapacity;
        if (ratio < 0.5f)
            return;
        Reserve(mCapacity << 1);
    }

    inline unsigned int hash(const K &key, unsigned int size) {
        unsigned int hsh = 0;
        if constexpr (std::is_pointer_v<K>) {
            unsigned int sz = sizeof(*key);
            if constexpr (sizeof(*key) == 1) {
                K kw = key;
                while (*kw != '\0') {
                    hsh = (hsh << 5) + (*kw++);
                }
            } else {
                const char *kw = ((char *) key);
                for (int i = 0; i < sz; i++) {
                    hsh = (hsh << 5) + (*kw++);
                }
            }
        } else if constexpr (std::is_same_v<K, TString<TAlloc>>) {
            auto keyO = (TString<TAlloc>) key;
            unsigned int sz = keyO.Length();
            const char *kw = keyO.Str();
            for (int i = 0; i < sz; i++) {
                hsh = (hsh << 5) + (*kw++);
            }
        } else {
            unsigned int sz = sizeof(key);
            const char *kw = ((char *) (&key));
            for (int i = 0; i < sz; i++) {
                hsh = (hsh << 5) + (*kw++);
            }
        }
        return ((mPrimeA * hsh + mPrimeB) % mPrimeC) & (size - 1);
    }

    inline int linearProbeSet(const K &key) {
        int index = hash(key, mCapacity);
        int i = 0;
        while (mProbes[index].used && mProbes[index].key != key && i <= mCapacity) {
            index = (index + 1) & (mCapacity - 1);
            i++;
        }
        if (mProbes[index].key != key && mProbes[index].used)
            return -1;
        return index;
    }

    inline int linearProbeGet(const K &key) {
        int index = hash(key, mCapacity);
        int i = 0;
        while (mProbes[index].key != key && i <= mCapacity) {
            index = (index + 1) & (mCapacity - 1);
            i++;
        }
        if (mProbes[index].key != key)
            return -1;
        return index;
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