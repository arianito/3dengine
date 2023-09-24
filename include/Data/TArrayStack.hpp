#pragma once

#include <cassert>

extern "C" {
#include "mem/utils.h"
}

#include "engine/Memory.hpp"

template<typename T, class TAlloc = FreeListMemory>
class TArrayStack {
private:
    T *mStack{nullptr};
    int mLength{0};
    int mCapacity{8};
    bool allowExpansion{true};
private:
    inline void expand() {
        if (mLength < mCapacity - 1)
            return;
        Reserve(mCapacity << 1);
    }
public:
    explicit inline TArrayStack() : TArrayStack(8) { allowExpansion = true; }

    explicit inline TArrayStack(int capacity) {
        mCapacity = capacity;
        mStack = Alloc<TAlloc, T>(mCapacity);
        allowExpansion = false;
    }

    explicit inline TArrayStack(const TArrayStack &) = delete;

    inline ~TArrayStack() {
        Free<TAlloc>(&mStack);
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

        memcpy(newList, mStack, nBytes);
        Free<TAlloc>(&mStack);

        mStack = newList;
        mCapacity = newCapacity;
    }


    inline void Push(const T &value) {
        if (mStack == nullptr)
            return;
        if (allowExpansion)expand();
        assert(mLength < mCapacity && "FixedStack: is full");
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
        Reserve(0);
    }

    inline bool Empty() {
        return mLength == 0;
    }
    inline bool Full() {
        return mLength == mCapacity - 1;
    }
};