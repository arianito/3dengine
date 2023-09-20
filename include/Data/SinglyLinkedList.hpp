#pragma once


#include "engine/Memory.hpp"
#include "engine/Object.hpp"

template<typename T>
class SinglyLinkedList {
public:
    struct Node {
        Node *next;
        T value;
    };
private:
    Node *mHead{nullptr};
    Allocator *mAllocator{nullptr};
    int mLength{0};
public:
    explicit inline SinglyLinkedList(Allocator *a) : mAllocator(a) {}

    explicit inline SinglyLinkedList(const SinglyLinkedList &) = delete;

    inline ~SinglyLinkedList() {
        clear();
    }


    inline void insertAfter(Node *node, const T &value) {
        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
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

    inline void remove(Node *node) {
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
            mAllocator->Free((void **) &it);
            return;
        }

        prev->next = it->next;
        mAllocator->Free((void **) &it);
    }


    inline void remove(const T &value) {
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
            mAllocator->Free((void **) &it);
            return;
        }

        prev->next = it->next;
        mAllocator->Free((void **) &it);
    }

    inline void pushFront(const T &value) {
        insertAfter(nullptr, value);
    }

    inline void pushBack(const T &value) {

        Node *it = mHead, *prev = nullptr;
        while (it != nullptr) {
            prev = it;
            it = it->next;
        }
        insertAfter(prev, value);
    }

    inline T popFront() {
        assert(mHead != nullptr && "LinkedList: is empty.\n");
        T value = mHead->value;
        remove(mHead);
        return value;
    }

    inline T popBack() {
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

    inline bool isEmpty() {
        return mHead == nullptr;
    }


    inline const int &size() {
        return mLength;
    }

    inline Node *head() const {
        return mHead;
    }

    inline Node *tail() const {
        return nullptr;
    }

};