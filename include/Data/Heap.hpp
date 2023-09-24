#pragma once

#include <cassert>
#include "engine/Memory.hpp"

static constexpr int MAX_HEAP = 1;
static constexpr int MIN_HEAP = 2;

template<typename T, class TAlloc>
class Heap {
private:
    T *mHeap{nullptr};
    int mCapacity{8};
    int mLength{0};
    int mPolicy{MAX_HEAP};

    inline void swap(int i, int j) {

        if (i == j)
            return;
        i -= 1;
        j -= 1;
        T tmp = mHeap[i];
        mHeap[i] = mHeap[j];
        mHeap[j] = tmp;
    }

    inline void expand() {
        if (mLength < mCapacity - 1)
            return;
        Reserve(mCapacity << 1);
    }


    inline int nextPowerOfTwo(int num) {
        if (num <= 0)
            return 0;

        num |= (num >> 1);
        num |= (num >> 2);
        num |= (num >> 4);
        num |= (num >> 8);
        num |= (num >> 16);

        return num + 1;
    }

    inline void sortUp(int index) {
        while (index > 0) {
            int parent = index / 2;

            if ((mPolicy == MAX_HEAP) ?
                (mHeap[parent - 1] > mHeap[index - 1]) :
                (mHeap[parent - 1] < mHeap[index - 1]))
                return;

            swap(index, parent);
            index = parent;
        }
    }

    inline void sortDown(int index) {
        while (index <= mLength) {

            int leftIndex = index * 2;
            int rightIndex = index * 2 + 1;

            // leaf node
            if (leftIndex > mLength)
                return;

            // assume left node is greater
            int next = leftIndex;
            //  check whether right node is greater or not
            if (rightIndex <= mLength &&
                ((mPolicy == MAX_HEAP) ?
                 (mHeap[rightIndex - 1] > mHeap[leftIndex - 1]) :
                 (mHeap[rightIndex - 1] < mHeap[leftIndex - 1]))) {
                next = rightIndex;
            }

            // check if placed correctly
            if ((mPolicy == MAX_HEAP) ?
                (mHeap[index - 1] > mHeap[next - 1]) :
                (mHeap[index - 1] < mHeap[next - 1]))
                return;


            swap(index, next);
            index = next;
        }
    }


    inline bool compare(int index, const T &value) {
        if (index > mLength)return true;
        if ((mPolicy == MAX_HEAP) ?
            (mHeap[index - 1] > value) :
            (mHeap[index - 1] < value))
            return false;
        return compare(index * 2, value) && compare(index * 2 + 1, value);
    }

public:
    explicit inline Heap(int policy) : mPolicy(policy) {
        mHeap = Alloc<TAlloc, T>(mCapacity);
    }

    explicit inline Heap(const Heap &) = delete;

    inline ~Heap() {
        Free<TAlloc>((void**) &mHeap);
    }

    inline void Reserve(int newCapacity) {
        T *newList = Alloc<TAlloc, T>(newCapacity);
        assert(newList != nullptr && "Heap: Insufficient memory.\n");
        int oldBytes = mCapacity * sizeof(T);
        memcpy(newList, mHeap, oldBytes);
        Free<TAlloc>((void**) &mHeap);
        mHeap = newList;
        mCapacity = newCapacity;
    }

    bool Empty() {
        return mLength == 0;
    }

    inline void Heapify(const T buff[], int n) {
        if (n > mCapacity) {
            int newCapacity = nextPowerOfTwo(n);
            T *newList = Alloc<TAlloc, T>(newCapacity);
            assert(newList != nullptr && "Heap: Insufficient memory.\n");
            Free<TAlloc>((void**) &mHeap);
            mCapacity = newCapacity;
            mHeap = newList;
        }
        mLength = n;
        int nBytes = mLength * sizeof(T);
        memcpy(mHeap, buff, nBytes);
        for (int i = mLength; i > 0; i--) {
            sortDown(i);
        }
    }


    void Push(const T &item) {
        expand();
        mHeap[mLength++] = item;

        sortUp(mLength);
    }

    T Pop() {
        assert(mLength > 0 && "Heap: is empty");

        T root = mHeap[0];
        mHeap[0] = mHeap[--mLength];

        sortDown(1);
        return root;
    }

    inline bool Validate(int index) {
        if (index > mLength) return true;

        return Validate(index * 2) &&
               Validate(index * 2 + 1) &&
               compare(index * 2, mHeap[index - 1]) &&
               compare(index * 2 + 1, mHeap[index - 1]);
    }
};