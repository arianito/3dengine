#pragma once

#include <cassert>

extern "C" {
#include "mem/utils.h"
}

#include "engine/Memory.hpp"

template<typename T, class TAlloc = FreeListMemory>
class FixedStack {
private:
    T *mStack{nullptr};
    int mLength{0};
    int mCapacity{8};
    bool allowExpansion{true};

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

        inline const T &operator*() const {
            return *ptr;
        }
    };

    Iterator begin() {
        return Iterator(mStack);
    }

    Iterator end() {
        return Iterator(mStack + mLength);
    }

private:

    inline void expand() {
        if (mLength < mCapacity - 1)
            return;
        Reserve(mCapacity << 1);
    }

    inline void shrink() {
        float ratio = (float) mLength / mCapacity;
        if ((ratio) > 0.25f)
            return;
        Reserve(mCapacity >> 1);
    }

    inline void copy(const FixedStack &other) {
        if (other.mLength > 0) {
            Reserve(other.mLength);
            memcpy(mStack, other.mStack, other.mLength);
            mLength = other.mLength;
        }
    }

public:
    explicit inline FixedStack() : FixedStack(8) { allowExpansion = true; }

    explicit inline FixedStack(int capacity) {
        mCapacity = capacity;
        mStack = Alloc<TAlloc, T>(mCapacity);
        allowExpansion = false;
    }


    explicit inline FixedStack(const FixedStack &) = delete;

    inline ~FixedStack() {
        Free<TAlloc>((void **) &mStack);
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
        Free<TAlloc>((void **) &mStack);

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