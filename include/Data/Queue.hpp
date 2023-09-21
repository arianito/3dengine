#pragma once

#include <cassert>

template<typename T>
class Queue {
public:
    struct Node {
        Node *next;
        T value;
    };
private:
    Allocator *mAllocator{nullptr};
    Node *mHead{nullptr};
    Node *mTail{nullptr};
    int mLength{0};
public:
    explicit inline Queue(Allocator *a) : mAllocator(a) {}

    explicit inline Queue(const Queue &) = delete;

    inline ~Queue() {
        clear();
    }

    inline void enqueue(const T &value) {
        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
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

    inline T dequeue() {
        assert(mHead != nullptr && "Queue: is empty");
        mLength--;
        Node *tmp = mHead;
        mHead = mHead->next;
        if (mHead == nullptr)
            mTail = nullptr;
        T value = tmp->value;
        mAllocator->Free((void **) &tmp);
        return value;
    }

    inline Node *head() const {
        return mHead;
    }

    inline Node *tail() const {
        return nullptr;
    }

    inline const T &current() {
        assert(mHead != nullptr && "Queue: is empty");
        return mHead->value;
    }


    inline bool empty() {
        return mHead == nullptr;
    }

    inline const int &size() {
        return mLength;
    }


    inline void clear() {
        Node *it = mHead;
        while (it != nullptr) {
            Node *tmp = it;
            it = it->next;
            mAllocator->Free((void **) &tmp);
        }
    }

};