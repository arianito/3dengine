#pragma once


#include "engine/Memory.hpp"

template<typename T, class TAlloc>
class TSinglyLinkedList {
private:
    struct Node {
        Node *next;
        T value;
    };
private:
    Node *mHead{nullptr};
    int mLength{0};
public:
    explicit inline TSinglyLinkedList() = default;

    explicit inline TSinglyLinkedList(const TSinglyLinkedList &) = delete;

    inline ~TSinglyLinkedList() {
        Clear();
    }


    inline void PushAfter(Node *node, const T &value) {
        Node *newNode = Alloc<TAlloc, Node>();
        newNode->next = nullptr;
        newNode->value = value;
        mLength++;
        if (node == nullptr) {
            newNode->next = mHead;
            mHead = newNode;
            return;
        }

        newNode->next = node->next;
        node->next = newNode;
    }

    inline void Remove(Node *node) {
        Node *it = mHead, *prev = nullptr;
        while (it != nullptr) {
            if (it == node)
                break;

            prev = it;
            it = it->next;
        }
        if (it == nullptr)
            return;

        mLength--;
        if (prev == nullptr) {
            mHead = it->next;
            Free<TAlloc>(&it);
            return;
        }

        prev->next = it->next;
        Free<TAlloc>(&it);
    }


    inline void Remove(const T &value) {
        Node *it = mHead, *prev = nullptr;
        while (it != nullptr) {
            if (it->value == value)
                break;

            prev = it;
            it = it->next;
        }
        if (it == nullptr)
            return;

        mLength--;
        if (prev == nullptr) {
            mHead = it->next;
            Free<TAlloc>(&it);
            return;
        }

        prev->next = it->next;
        Free<TAlloc>(&it);
    }

    inline void PushFront(const T &value) {
        insertAfter(nullptr, value);
    }

    inline void PushBack(const T &value) {

        Node *it = mHead, *prev = nullptr;
        while (it != nullptr) {
            prev = it;
            it = it->next;
        }
        insertAfter(prev, value);
    }

    inline T PopFront() {
        assert(mHead != nullptr && "LinkedList: is empty.\n");
        T value = mHead->value;
        remove(mHead);
        return value;
    }

    inline T PopBack() {
        assert(mHead != nullptr && "LinkedList: is empty.\n");
        Node *it = mHead, *prev = nullptr;
        while (it != nullptr) {
            prev = it;
            it = it->next;
        }

        T value = prev->value;
        remove(prev);
        return std::move(value);
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

    inline T &operator[](int index) {
        assert(index >= 0 && index < mLength &&
               "LinkedList: Index out of range.\n");

        Node *it = mHead;
        int i = 0;
        while (it != nullptr) {
            if (i == index)
                return it->value;
            it = it->next;
            i++;
        }
    }

    inline bool Empty() {
        return mHead == nullptr;
    }


    inline const int &Length() {
        return mLength;
    }
};