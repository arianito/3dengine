#pragma once

#include <cassert>
#include "engine/Memory.hpp"
#include "engine/Object.hpp"

template<typename T>
class FixedStack : public Object<FixedStack<T>> {
public:
    Allocator *mAllocator;
    T *mStack{nullptr};
    int mLength{0};
    int mCapacity{8};

    explicit inline FixedStack(Allocator *a, int capacity) :
            mAllocator(a),
            mCapacity(capacity) {
        mStack = (T *) mAllocator->Alloc(mCapacity * sizeof(T));
    }

    explicit inline FixedStack(const FixedStack &) = delete;

    inline ~FixedStack() {
        mAllocator->Free((void **) &mStack);
    }


    inline void push(const T &value) {
        assert(mLength < mCapacity && "FixedStack: is full");
        mStack[mLength++] = value;
    }

    inline T pop() {
        assert(mLength > 0 && "FixedStack: is empty");
        return mStack[--mLength];
    }

    inline void clear() {
        mLength = 0;
    }

    inline bool empty() {
        return mLength == 0;
    }
};