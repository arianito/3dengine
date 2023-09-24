#pragma once

#include <cassert>

#include "engine/Memory.hpp"

template<typename T, class TAlloc>
class TQueue {
public:
    struct Node {
        Node *next;
        T value;
    };
private:
    Node *mHead{nullptr};
    Node *mTail{nullptr};
    int mLength{0};
public:
    explicit inline TQueue() {}

    explicit inline TQueue(const TQueue &) = delete;

    inline ~TQueue() {
        Clear();
    }

    inline void Enqueue(const T &value) {
        Node *newNode = Alloc<TAlloc, Node>();
        newNode->next = nullptr;
        newNode->value = value;
        mLength++;

        if (mHead == nullptr) {
            mHead = newNode;
            mTail = newNode;
            return;
        }
        mTail->next = newNode;
        mTail = newNode;
    }

    inline T Dequeue() {
        assert(mHead != nullptr && "Queue: is empty");
        mLength--;
        Node *tmp = mHead;
        mHead = mHead->next;
        if (mHead == nullptr)
            mTail = nullptr;
        T value = tmp->value;
        Free<TAlloc>(&tmp);
        return value;
    }

    inline const T &Front() {
        assert(mHead != nullptr && "Queue: is empty");
        return mHead->value;
    }


    inline bool Empty() {
        return mHead == nullptr;
    }

    inline const int &Length() {
        return mLength;
    }


    inline void Clear() {
        Node *it = mHead;
        while (it != nullptr) {
            Node *tmp = it;
            it = it->next;
            Free<TAlloc>(&tmp);
        }
    }

};