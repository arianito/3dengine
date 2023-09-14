#pragma  once


#include "cassert"

#include "engine/Object.hpp"
#include "engine/Memory.hpp"


template<typename T>
class Stack : Object<Stack<T>> {
public:
    struct Node {
        Node *next;
        T value;
    };
private:
    Allocator *mAllocator{nullptr};
    Node *mHead;
    int mLength{0};
public:
    explicit  inline Stack(Allocator *a) : mAllocator(a) {}

    explicit inline Stack(const Stack &) = delete;

    inline ~Stack() {
        clear();
    }

    inline void push(const T &value) {
        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
        newNode->next = mHead;
        newNode->value = value;
        mHead = newNode;
        mLength++;
    }

    inline T pop() {
        assert(mHead != nullptr && "Stack: is empty");
        Node *tmp = mHead;
        mHead = mHead->next;
        T value = tmp->value;
        mAllocator->Free((void **) &tmp);
        mLength--;
        return value;
    }


    inline Node *head() const {
        return mHead;
    }

    inline Node *tail() const {
        return nullptr;
    }

    inline bool empty() {
        return mHead == nullptr;
    }

    inline const T &current() {
        assert(mHead != nullptr && "Stack: is empty");
        return mHead->value;
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
        mHead = nullptr;
        mLength = 0;
    }
};