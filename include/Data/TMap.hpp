#pragma once


#include <cassert>

#include "data/TString.hpp"
#include "engine/Memory.hpp"

template<typename K, typename V, class TAlloc>
class TMap {
private:
    struct Node {
        Node *next;
        K key;
        V value;
    };
private:
    static constexpr size_t mPrimeA{492366587};
    static constexpr size_t mPrimeB{1645333507};
    static constexpr size_t mPrimeC{6692367337};
    unsigned int mDefaultStart{8};
    unsigned int mBucketCount{mDefaultStart};
    Node **mBuckets;
    unsigned int mLength = 0;
    Node dummy;
private:
    inline void expand() {
        float ratio = (float) mLength / mBucketCount;
        if (ratio < 0.5f)
            return;
        Reserve(mBucketCount << 1);
    }
public:
    explicit inline TMap() {
        mBuckets = Alloc<TAlloc, Node *, true>(mBucketCount);
    }

    explicit inline TMap(const TMap &) = delete;

    ~TMap() {
        for (int i = 0; i < mBucketCount; i++) {
            Node *it = (Node *) mBuckets[i];
            while (it != nullptr) {
                Node *tmp = it;
                it = it->next;
                Free<TAlloc>(&tmp);
            }
        }
        Free<TAlloc>(&mBuckets);
    }

    inline void Reserve(int newCapacity) {
        Node **newList = Alloc<TAlloc, Node *, true>(newCapacity);
        for (int i = 0; i < mBucketCount; i++) {
            auto it = mBuckets[i];
            while (it != nullptr) {
                unsigned int hsh = hash(it->key, newCapacity);
                Node *tmp = it;
                it = it->next;
                tmp->next = newList[hsh];
                newList[hsh] = tmp;
            }
        }

        Free<TAlloc>(&mBuckets);
        mBuckets = newList;
        mBucketCount = newCapacity;
    }


    inline void Set(const K &key, const V &value) {
        expand();
        unsigned int hsh = hash(key, mBucketCount);
        Node *node = mBuckets[hsh];

        while (node != nullptr) {
            if (node->key == key)
                break;
            node = node->next;
        }

        if (node != nullptr) {
            node->value = value;
            return;
        }

        Node *newNode = Alloc<TAlloc, Node>();
        newNode->next = nullptr;
        newNode->key = key;
        newNode->value = value;

        newNode->next = mBuckets[hsh];
        mBuckets[hsh] = newNode;
        mLength++;
    }

    inline void Remove(const K &key) {
        unsigned int hsh = hash(key, mBucketCount);

        Node *node = mBuckets[hsh],
                *prev = nullptr;

        while (node != nullptr) {
            if (node->key == key)
                break;
            prev = node;
            node = node->next;
        }
        if (node == nullptr)
            return;


        if (prev == nullptr) {
            Node *tmp = mBuckets[hsh];
            mBuckets[hsh] = tmp->next;
            Free<TAlloc>(&tmp);
            mLength--;
            return;
        }
        prev->next = node->next;
        Free<TAlloc>(&node);
        mLength--;
    }

    inline V &operator[](const K &key) {
        unsigned int hsh = hash(key, mBucketCount);
        Node *node = mBuckets[hsh];
        while (node != nullptr) {
            if (node->key == key)
                break;
            node = node->next;
        }
        if (node == nullptr) {
            return dummy.value;
        }
        return node->value;
    }

    inline const unsigned int &Length() {
        return mLength;
    }

    inline void Clear() {
        for (int i = 0; i < mBucketCount; i++) {
            auto it = mBuckets[i];
            while (it != nullptr) {
                Node *tmp = it;
                it = it->next;
                Free<TAlloc>(&tmp);
            }
        }
        Free<TAlloc>(&mBuckets);
        mBucketCount = mDefaultStart;
        mBuckets = Alloc<TAlloc, Node*, true>(mBucketCount);
        mLength = 0;
    }
};