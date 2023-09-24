#pragma  once

#include "engine/Memory.hpp"


template<typename T, class TAlloc>
class TDoublyLinkedList {
private:
    struct Node {
        Node *next;
        Node *prev;
        T value;
    };
private:
    Node *mHead{nullptr};
    Node *mTail{nullptr};
    int mLength{0};

public:
    explicit inline TDoublyLinkedList() {
        mHead = Alloc<TAlloc, Node>();
        mTail = Alloc<TAlloc, Node>();
        mHead->prev = nullptr;
        mHead->next = mTail;
        mTail->prev = mHead;
        mTail->next = nullptr;
    }

    explicit inline TDoublyLinkedList(const TDoublyLinkedList &) = delete;

    inline ~TDoublyLinkedList() {
        Clear();
        Free<TAlloc>(&mHead);
        Free<TAlloc>(&mTail);
    }

    inline void InsertAfter(Node *node, const T &value) {
        Node *newNode = Alloc<TAlloc, Node>();
        newNode->value = value;
        Node *next = node->next;
        newNode->prev = node;
        newNode->next = next;
        node->next = newNode;
        next->prev = newNode;
        mLength++;

    }

    inline void InsertBefore(Node *node, const T &value) {
        Node *newNode = Alloc<TAlloc, Node>();
        newNode->value = value;
        Node *prev = node->prev;
        newNode->next = node;
        newNode->prev = prev;
        node->prev = newNode;
        prev->next = newNode;
        mLength++;
    }

    inline void Remove(Node *node) {
        assert(node->next != nullptr && node->prev != nullptr &&
               "DoublyLinkedList: node not exist");

        Node *prev = node->prev;
        Node *next = node->next;
        prev->next = node->next;
        next->prev = node->prev;
        Free<TAlloc>(&node);
        mLength--;

    }

    inline void Remove(const T &value) {
        Node *it = mHead->next;
        while (it != mTail) {
            if (it->value == value) break;
            it = it->next;
        }
        Remove(it);
    }

    inline bool Search(const T &value) {
        Node *it = mHead->next;
        while (it != mTail) {
            if (it->value == value) return true;
            it = it->next;
        }
        return false;
    }

    inline void PushFront(const T &value) {
        insertAfter(mHead, value);
    }

    inline void PushBack(const T &value) {

        insertBefore(mTail, value);
    }

    inline T PopFront() {
        assert(mHead->next != nullptr && mHead->next != mTail &&
               "DoublyLinkedList: is empty");
        T value = mHead->next->value;
        remove(mHead->next);
        return value;
    }

    inline T PopBack() {
        assert(mTail->prev != nullptr && mTail->prev != mHead &&
               "DoublyLinkedList: is empty");
        T value = mTail->prev->value;
        remove(mTail->prev);
        return value;

    }

    inline T &operator[](int index) {
        assert(index >= 0 && index < mLength &&
               "DoublyLinkedList: index out of range");
        Node *it = mHead->next;
        int i = 0;
        while (it != mTail) {
            if (i == index)
                break;
            i++;
            it = it->next;
        }
        return it->value;
    }


    inline const int &Length() {
        return mLength;
    }

    inline void Clear() {
        Node *it = mHead->next;
        while (it != mTail) {
            Node *tmp = it;
            it = it->next;
            Free<TAlloc>(&tmp);
        }
        mHead->next = mTail;
        mTail->prev = mHead;
        mLength = 0;
    }
};