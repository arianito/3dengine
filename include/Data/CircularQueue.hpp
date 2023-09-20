#pragma once


#include "cassert"
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

template<typename T>
class CircularQueue {
private:
    Allocator *mAllocator{nullptr};
public:
    int mTail{0};
    int mHead{0};
    T *mQueue{nullptr};
    int mCapacity{8};
public:
    explicit inline CircularQueue(Allocator *a, int capacity) :
            mAllocator(a),
            mCapacity(capacity + 1) {
        mQueue = (T *) mAllocator->Alloc(mCapacity * sizeof(T));
    }

    explicit inline CircularQueue(const CircularQueue &) = delete;

    ~CircularQueue() {
        mAllocator->Free((void **) &mQueue);
    }

    inline void enqueue(const T &value) {
        if (((mTail + 1) % mCapacity) == mHead)
            return;
        mQueue[mTail] = value;
        mTail = (mTail + 1) % mCapacity;
    }

    inline T dequeue() {
        assert(mTail != mHead && "Circular Queue: is empty");
        T value = mQueue[mHead];
        memset(&mQueue[mHead], 0, sizeof(T));
        mHead = (mHead + 1) % mCapacity;
        return value;
    }


    inline int size() {
        if (mHead == mTail)
            return 0;
        int d = mTail - mHead;
        if (d < 0)
            d += mCapacity;
        return d;
    }

    inline void clear() {
        mHead = 0;
        mTail = 0;
    }

    inline bool empty() {
        return mTail == mHead;
    }
};