#pragma once

#include <cassert>

#include "engine/Object.hpp"
#include "engine/Memory.hpp"

template<typename K, typename V>
class ProbeHashTable : public Object<ProbeHashTable<K, V>> {
public:
    struct Node {
        K key;
        V value;
        bool used;
    };
public:
    static constexpr size_t mPrimeA{492366587};
    static constexpr size_t mPrimeB{1645333507};
    static constexpr size_t mPrimeC{6692367337};
    Allocator *mAllocator{nullptr};
    Node *mProbes{nullptr};
    unsigned int mDefaultStart{8};
    unsigned int mCapacity{mDefaultStart};
    unsigned int mLength{0};
    Node dummy;
public:
    explicit inline ProbeHashTable(Allocator *a) : mAllocator(a) {
        unsigned int nBytes = mCapacity * sizeof(Node);
        mProbes = (Node *) mAllocator->Alloc(nBytes);
        memset(mProbes, 0, nBytes);
    }

    explicit inline ProbeHashTable(const ProbeHashTable &) = delete;

    inline ~ProbeHashTable() {
        mAllocator->Free((void **) &mProbes);
    }

    inline void expand() {
        float ratio = (float) mLength / mCapacity;
        if (ratio < 1.0f)
            return;

        unsigned int newCapacity = mCapacity << 1;
        unsigned int nBytes = newCapacity * sizeof(Node);

        Node *newList = (Node *) mAllocator->Alloc(nBytes);
        memset(newList, 0, nBytes);


        for (int i = 0; i < mCapacity; i++) {
            if (mProbes[i].used) {
                Node probe = mProbes[i];
                unsigned int index = hash(probe.key, newCapacity);
                while (newList[index].used && newList[index].key != probe.key) {
                    index = (index + 1) % newCapacity;
                }
                newList[index] = {probe.key, probe.value, probe.used};
            }
        }

        mAllocator->Free((void **) &mProbes);
        mProbes = newList;
        mCapacity = newCapacity;

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
        if(mProbes[index].key != key && mProbes[index].used)
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
        if(mProbes[index].key != key)
            return -1;
        return index;
    }

    inline void set(const K &key, const V &value) {
        expand();
        int index = linearProbeSet(key);
        if(index < 0)
            return;
        if (!mProbes[index].used)
            mLength++;
        mProbes[index] = {key, value, true};

    }

    inline void remove(const K &key) {
        int index = linearProbeGet(key);
        if(index < 0)
            return;
        if (mProbes[index].used && mProbes[index].key == key) {
            mProbes[index].used = false;
            mLength--;
        }
    }

    inline V &operator[](const K &key) {
        int index = linearProbeGet(key);
        if(index < 0)
            return dummy.value;
        return mProbes[index].value;
    }

    inline unsigned int &size() {
        return mLength;
    }

    inline void clear() {
        mAllocator->Free((void **) &mProbes);
        mCapacity = mDefaultStart;
        unsigned int nBytes = mCapacity * sizeof(Node);
        mProbes = (Node *) mAllocator->Alloc(nBytes);
        memset(mProbes, 0, nBytes);
        mLength = 0;
    }
};