#pragma  once


#include <cassert>
#include "engine/Object.hpp"
#include "engine/Memory.hpp"


template<typename T>
class Deque : public Object<Deque<T>> {
public:
    struct Node {
        Node *next;
        Node *prev;
        T value;
    };
private:
    Allocator *mAllocator;
    Node *mHead;
    Node *mTail;
    int mLength;
public:
    explicit inline Deque(Allocator *a) : mAllocator(a) {
        mHead = (Node*)mAllocator->Alloc(sizeof (Node));
        mTail = (Node*)mAllocator->Alloc(sizeof (Node));
        mHead->prev = nullptr;
        mHead->next = mTail;
        mTail->prev = mHead;
        mTail->next = nullptr;
    }

    explicit  inline Deque(const Deque &) = delete;

    ~Deque() {
        clear();
    }


    inline void pushBack(const T& value) {

        Node* newNode = (Node*)mAllocator->Alloc(sizeof (Node));
        newNode->next = mTail;
        newNode->value = value;

        Node* prev = mTail->prev;
        mTail->prev = newNode;
        newNode->next = mTail;
        newNode->prev = prev;
        prev->next = newNode;
        mLength++;

    }

    inline void pushFront(const T& value) {

        Node* newNode = (Node*)mAllocator->Alloc(sizeof (Node));
        newNode->next = mTail;
        newNode->value = value;

        Node* next = mHead->next;

        mHead->next = newNode;
        newNode->next = next;
        newNode->prev = mHead;
        next->prev = newNode;
        mLength++;
    }

    inline T popFront() {
        assert(mHead->next != mTail && mTail->prev != mHead && "Deque: is empty");
        Node* node = mHead->next;

        mHead->next = node->next;
        node->next->prev = mHead;

        T value = node->value;
        mAllocator->Free((void**) &node);
        mLength--;
        return value;
    }

    inline T popBack() {
        assert(mHead->next != mTail && mTail->prev != mHead && "Deque: is empty");
        Node* node = mTail->prev;

        mTail->prev = node->prev;
        node->prev->next = mTail;

        T value = node->value;
        mAllocator->Free((void**) &node);
        mLength--;
        return value;
    }

    inline Node* head() const {
        return mHead->next;
    }

    inline Node* tail() const {
        return mTail;
    }

    inline const int& size() {
        return mLength;
    }

    inline void clear() {
        Node* it = mHead->next;
        while(it != mTail) {
            Node* tmp = it;
            it = it->next;
            mAllocator->Free((void**) &tmp);
        }
        mHead->next = mTail;
        mTail->prev = mHead;
        mLength = 0;
    }
};