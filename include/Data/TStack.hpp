#pragma  once


#include "cassert"

#include "engine/Memory.hpp"


template<typename T, class TAlloc>
class TStack {
private:
    struct Node {
        Node *next;
        T value;
    };
private:
    Node *mHead;
    int mLength{0};
public:
    explicit  inline TStack() = default;

    explicit inline TStack(const TStack &) = delete;

    inline ~TStack() { Clear(); }

    inline void Push(const T &value) {
        Node *newNode = Alloc<TAlloc, Node>();
        newNode->next = mHead;
        newNode->value = value;
        mHead = newNode;
        mLength++;
    }

    inline T Pop() {
        assert(mHead != nullptr && "Stack: is empty");
        Node *tmp = mHead;
        mHead = mHead->next;
        T value = tmp->value;
        Free<TAlloc>(&tmp);
        mLength--;
        return value;
    }

    inline bool Empty() {
        return mHead == nullptr;
    }

    inline const T &Front() {
        assert(mHead != nullptr && "Stack: is empty");
        return mHead->value;
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
        mHead = nullptr;
        mLength = 0;
    }
};