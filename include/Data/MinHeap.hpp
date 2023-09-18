#pragma once

#include "engine/Object.hpp"
#include "engine/Memory.hpp"

template<typename T>
class MinHeap {

public:
    Allocator *mAllocator{nullptr};
    T *mHeap{nullptr};
    int mDefaultCapacity{16};
    int mCapacity{mDefaultCapacity};
    int mLength{0};

    explicit inline MinHeap(Allocator *a) : mAllocator(a) {
        mHeap = (T *) mAllocator->Alloc(mCapacity * sizeof(T));
    }

    explicit inline MinHeap(const MinHeap &) = delete;

    inline ~MinHeap() {
        mAllocator->Free((void **) &mHeap);
    }

    inline void expand() {
        if (mLength * 2 < mCapacity)
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

            if (mHeap[parentIndex - 1] <= mHeap[index - 1])
                return;

            swap(parentIndex, index);
            index = parentIndex;
        }

    }

    T pop() {
        assert(mLength > 0 && "MinHeap: is empty");

        T root = mHeap[0];
        mHeap[0] = mHeap[--mLength];

        int index = 1;
        while (index <= mLength) {

            int leftIndex = index * 2;
            int rightIndex = index * 2 + 1;

            // leaf node
            if (leftIndex > mLength)
                return root;

            // assume left node is lesser
            int next = leftIndex;
            //  check whether right node is lesser or not
            if (rightIndex <= mLength &&
                mHeap[rightIndex - 1] < mHeap[leftIndex - 1]) {
                next = rightIndex;
            }

            // check if placed correctly
            if (mHeap[index - 1] < mHeap[next - 1])
                return root;


            swap(index, next);
            index = next;
        }
        return root;
    }

    inline bool isGreater(int index, const T &value) {
        if (index > mLength)return true;
        if (mHeap[index - 1] < value) return false;
        return isGreater(index * 2, value) && isGreater(index * 2 + 1, value);
    }

    inline bool validate(int index) {
        if (index > mLength) return true;

        return validate(index * 2) &&
               validate(index * 2 + 1) &&
               isGreater(index * 2, mHeap[index - 1]) &&
               isGreater(index * 2 + 1, mHeap[index - 1]);
    }
};