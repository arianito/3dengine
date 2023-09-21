#pragma once

#include <cassert>
#include "engine/Memory.hpp"

template<typename T, class TAlloc = FreeListMemory>
class FixedStack {
private:
    T *mStack{nullptr};
    int mLength{0};
    int mCapacity{8};
private:
    inline void expand() {
        if ((mLength << 1) < mCapacity)
            return;
        Reserve(mCapacity << 1);
    }

    inline void copy(const FixedStack &other) {
        if (other.mLength > 0) {
            Reserve(other.mLength);
            memcpy(mStack, other.mStack, other.mLength);
            mLength = other.mLength;
        }
    }

public:
    explicit inline FixedStack() {
        mStack = Alloc<TAlloc, T>(mCapacity);
    }

    explicit inline FixedStack(const FixedStack &) = delete;

    inline ~FixedStack() {
        Free<TAlloc>((void **) &mStack);
    }

    inline void Fit() {
        Reserve(mLength);
    }

    inline void Reserve(int newCapacity) {
        if (newCapacity < mLength)
            return;

        int nBytes = mLength * sizeof(T);

        T *newList = Alloc<TAlloc, T>(newCapacity);
        assert(newList != nullptr && "Array: Insufficient memory.\n");

        memcpy(newList, mStack, nBytes);
        Free<TAlloc>((void **) &mStack);

        mStack = newList;
        mCapacity = newCapacity;
    }


    inline void Push(const T &value) {
        assert(mLength < mCapacity && "FixedStack: is full");
        expand();
        mStack[mLength++] = value;
    }

    inline const T &Front() {
        assert(mLength > 0 && "FixedStack: is empty");
        return mStack[mLength];
    }

    inline T Pop() {
        assert(mLength > 0 && "FixedStack: is empty");
        return mStack[--mLength];
    }

    inline void Clear() {
        mLength = 0;
    }

    inline bool Empty() {
        return mLength == 0;
    }
};