#pragma once


#include <cassert>

#include "engine/Object.hpp"
#include "engine/Memory.hpp"

template<typename K, typename V>
class HashTable {
public:
    struct Node {
        Node *next;
        K key;
        V value;
    };
//private:
    static constexpr size_t mPrimeA{492366587};
    static constexpr size_t mPrimeB{1645333507};
    static constexpr size_t mPrimeC{6692367337};
    unsigned int mDefaultStart{8};
    unsigned int mBucketCount{mDefaultStart};
    Node **mBuckets;
    Allocator *mAllocator{nullptr};
    unsigned int mLength = 0;
    Node dummy;
public:
    explicit inline HashTable(Allocator *a) : mAllocator(a) {
        unsigned int nBytes = mBucketCount * sizeof(Node *);
        mBuckets = (Node **) mAllocator->Alloc(nBytes);
        memset(mBuckets, 0, nBytes);
    }

    explicit inline HashTable(const HashTable &) = delete;

    ~HashTable() {
        for (int i = 0; i < mBucketCount; i++) {
            Node *it = (Node *) mBuckets[i];
            while (it != nullptr) {
                Node *tmp = it;
                it = it->next;
                mAllocator->Free((void **) &tmp);
            }
        }
        mAllocator->Free((void **) &mBuckets);
    }


    inline void expand() {
        float ratio = (float) mLength / mBucketCount;
        if (ratio < 0.75)
            return;

        unsigned int newSize = mBucketCount << 1;

        unsigned int nBytes = newSize * sizeof(Node *);
        Node **newList = (Node **) mAllocator->Alloc(nBytes);
        memset(newList, 0, nBytes);
        for (int i = 0; i < mBucketCount; i++) {
            auto it = mBuckets[i];
            while (it != nullptr) {
                unsigned int hsh = hash(it->key, newSize);
                Node* tmp = it;
                it = it->next;
                tmp->next = newList[hsh];
                newList[hsh] = tmp;
            }
        }

        mAllocator->Free((void **) &mBuckets);
        mBuckets = newList;
        mBucketCount = newSize;
    }

    inline void set(const K &key, const V &value) {
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

        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
        newNode->next = nullptr;
        newNode->key = key;
        newNode->value = value;

        newNode->next = mBuckets[hsh];
        mBuckets[hsh] = newNode;
        mLength++;
    }

    inline void remove(const K &key) {

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
            Node* tmp = mBuckets[hsh];
            mBuckets[hsh] = tmp->next;
            mAllocator->Free((void **) &tmp);
            mLength--;
            return;
        }
        prev->next = node->next;
        mAllocator->Free((void **) &node);
        mLength--;
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

    inline const unsigned int &size() {
        return mLength;
    }

    inline void clear() {
        for (int i = 0; i < mBucketCount; i++) {
            auto it = mBuckets[i];
            while (it != nullptr) {
                Node *tmp = it;
                it = it->next;
                mAllocator->Free((void **) &tmp);
            }
        }
        mAllocator->Free((void **) &mBuckets);
        mBucketCount = mDefaultStart;
        int nBytes = mBucketCount * sizeof(Node *);
        mBuckets = (Node **) mAllocator->Alloc(nBytes);
        memset(mBuckets, 0, nBytes);
        mLength = 0;
    }
};