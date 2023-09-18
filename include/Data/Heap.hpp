#pragma once

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

    inline void expand() {
        if (mLength < mCapacity - 1)
            return;

        int nBytes = mCapacity * sizeof(T);
        T *newList = (T *) mAllocator->Alloc(nBytes * 2);
        assert(newList != nullptr && "Array: Insufficient memory.\n");
        memset(newList, 0, nBytes * 2);
        memcpy(newList, mHeap, nBytes);
        mAllocator->Free((void **) (&mHeap));
        mHeap = newList;
        mCapacity = mCapacity * 2;
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


    void push(T item) {
        expand();
        mHeap[mLength++] = item;

        int index = mLength;
        while (index > 1) {
            int parentIndex = index / 2;

            if ((mPolicy == MAX_HEAP) ?
                (mHeap[parentIndex - 1] > mHeap[index - 1]) :
                (mHeap[parentIndex - 1] < mHeap[index - 1]))
                return;

            swap(parentIndex, index);
            index = parentIndex;
        }

    }

    T pop() {
        assert(mLength > 0 && "Heap: is empty");

        T root = mHeap[0];
        mHeap[0] = mHeap[--mLength];

        int index = 1;
        while (index <= mLength) {

            int leftIndex = index * 2;
            int rightIndex = index * 2 + 1;

            // leaf node
            if (leftIndex > mLength)
                return root;

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
                return root;


            swap(index, next);
            index = next;
        }
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