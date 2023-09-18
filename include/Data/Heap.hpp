#pragma once

#include <cassert>
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

static constexpr int MAX_HEAP = 1;
static constexpr int MIN_HEAP = 2;

template<typename T>
class Heap {

public:
    Allocator *mAllocator{nullptr};
    T *mHeap{nullptr};
    int mDefaultCapacity{8};
    int mCapacity{mDefaultCapacity};
    int mLength{0};
    int mPolicy{MAX_HEAP};

    explicit inline Heap(Allocator *a, int policy) : mAllocator(a),
                                                     mPolicy(policy) {
        mHeap = (T *) mAllocator->Alloc(mCapacity * sizeof(T));
    }

    explicit inline Heap(const Heap &) = delete;

    inline ~Heap() {
        mAllocator->Free((void **) &mHeap);
    }

    inline void reserve(int newCapacity) {
        int newBytes = newCapacity * sizeof(T);
        int oldBytes = mCapacity * sizeof(T);
        T *newList = (T *) mAllocator->Alloc(newBytes);
        assert(newList != nullptr && "Heap: Insufficient memory.\n");
        memcpy(newList, mHeap, oldBytes);
        mAllocator->Free((void **) (&mHeap));
        mHeap = newList;
        mCapacity = newCapacity;
    }

    inline void expand() {
        if (mLength < mCapacity - 1)
            return;
        reserve(mCapacity << 1);
    }

    inline void swap(int i, int j) {

        if (i == j)
            return;
        i -= 1;
        j -= 1;
        T tmp = mHeap[i];
        mHeap[i] = mHeap[j];
        mHeap[j] = tmp;
    }

    bool empty() {
        return mLength == 0;
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

    inline void heapify(const T buff[], int n) {
        if (n > mCapacity) {
            int newCapacity = nextPowerOfTwo(n);
            int newBytes = newCapacity * sizeof(T);
            T *newList = (T *) mAllocator->Alloc(newBytes);
            assert(newList != nullptr && "Heap: Insufficient memory.\n");
            mAllocator->Free((void **) (&mHeap));
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


    void push(const T &item) {
        expand();
        mHeap[mLength++] = item;

        sortUp(mLength);
    }

    T pop() {
        assert(mLength > 0 && "Heap: is empty");

        T root = mHeap[0];
        mHeap[0] = mHeap[--mLength];

        sortDown(1);
        return root;
    }

    inline bool compare(int index, const T &value) {
        if (index > mLength)return true;
        if ((mPolicy == MAX_HEAP) ?
            (mHeap[index - 1] > value) :
            (mHeap[index - 1] < value))
            return false;
        return compare(index * 2, value) && compare(index * 2 + 1, value);
    }

    inline bool validate(int index) {
        if (index > mLength) return true;

        return validate(index * 2) &&
               validate(index * 2 + 1) &&
               compare(index * 2, mHeap[index - 1]) &&
               compare(index * 2 + 1, mHeap[index - 1]);
    }
};