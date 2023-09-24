#pragma once


#include "cassert"
#include "engine/Memory.hpp"

template<typename T, class TAlloc>
class TArrayQueue {
private:
    T *mQueue{nullptr};
    int mTail{0};
    int mHead{0};
    int mCapacity{8};
public:
    explicit inline TArrayQueue(int capacity) : mCapacity(capacity + 1) {
        mQueue = Alloc<TAlloc, T>(mCapacity);
    }

    explicit inline TArrayQueue(const TArrayQueue &) = delete;

    ~TArrayQueue() {
        Free<TAlloc>(&mQueue);
    }

    inline void Enqueue(const T &value) {
        if (((mTail + 1) % mCapacity) == mHead)
            return;
        mQueue[mTail] = value;
        mTail = (mTail + 1) % mCapacity;
    }

    inline T Dequeue() {
        assert(mTail != mHead && "Circular Queue: is empty");
        T value = mQueue[mHead];
        memset(&mQueue[mHead], 0, sizeof(T));
        mHead = (mHead + 1) % mCapacity;
        return value;
    }


    inline int Length() {
        if (mHead == mTail)
            return 0;
        int d = mTail - mHead;
        if (d < 0)
            d += mCapacity;
        return d;
    }

    inline void Clear() {
        mHead = 0;
        mTail = 0;
    }

    inline bool Empty() {
        return mTail == mHead;
    }
};