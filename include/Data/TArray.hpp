#pragma once

#include <utility>
#include <cassert>

extern "C" {
#include "mem/utils.h"
}

#include "engine/Memory.hpp"

template<typename T, class TAlloc = FreeListMemory>
class TArray {
private:
    T *mList;
    unsigned int mCapacity{8};
    int mLength{0};
public:

    class Iterator {
    private:
        T *ptr;

    public:
        explicit inline Iterator(T *ptr) : ptr(ptr) {}

        inline Iterator &operator++() {
            ++ptr;
            return *this;
        }

        inline bool operator!=(const Iterator &other) const {
            return ptr != other.ptr;
        }

        inline T &operator*() {
            return *ptr;
        }
    };

    Iterator begin() {
        return Iterator(mList);
    }

    Iterator end() {
        return Iterator(mList + mLength);
    }

private:

    inline void expand() {
        if (mLength < mCapacity - 1)
            return;
        Reserve(mCapacity << 1);
    }


public:
    explicit inline TArray() : TArray(8) {}

    inline TArray(unsigned int capacity) : mCapacity(capacity), mLength(0) {
        mList = Alloc<TAlloc, T>(mCapacity);
    }

    explicit inline TArray(const TArray &) = delete;

    inline ~TArray() {
        Free<TAlloc>(&mList);
    }

    inline void Clear() {
        mLength = 0;
        Reserve(8);
    }

    inline void Fit() {
        Reserve(NEXTPOW2(mLength));
    }

    inline void Reserve(int newCapacity) {
        if (newCapacity < mLength)
            return;

        int nBytes = mLength * sizeof(T);


        T *newList = Alloc<TAlloc, T>(newCapacity);
        assert(newList != nullptr && "Array: Insufficient memory.\n");

        memcpy(newList, mList, nBytes);
        Free<TAlloc>(&mList);

        mList = newList;
        mCapacity = newCapacity;
    }

    inline void Remove(unsigned int index) {
        assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
        for (unsigned int i = index; i < mLength - 1; i++)
            mList[i] = mList[i + 1];
        mLength--;
    }

    inline T Pop() {
        assert(mLength > 0 && "Array: is empty.\n");
        return mList[--mLength];
    }

    inline void Add(const T &element) {
        Insert(element, mLength);
    }

    inline void Insert(const T &element, unsigned int index) {
        assert(index >= 0 && index <= mLength && "Array: Index out of range.\n");
        expand();

        for (unsigned int i = mLength; i > index; i--)
            mList[i] = mList[i - 1];

        mList[index] = element;
        mLength++;
    }

    inline T &operator[](unsigned int index) {
        assert(index >= 0 && index < mLength && "Array: Index out of range.\n");
        return mList[index];
    }

    inline int Find(const T &obj) {
        for (int i = 0; i < mLength; i++) {
            if (mList[i] == obj)
                return i;
        }
        return -1;
    }

    inline T *Ptr() const {
        return mList;
    }

    inline bool Empty() {
        return mLength == 0;
    }

    inline const int &Length() {
        return mLength;
    }

    inline const unsigned int &Capacity() {
        return mCapacity;
    }
};