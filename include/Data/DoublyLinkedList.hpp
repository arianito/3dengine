#pragma  once

#include "engine/Memory.hpp"
#include "engine/Object.hpp"


template<typename T>
class DoublyLinkedList : public Object<DoublyLinkedList<T>> {
public:
    struct Node {
        Node *next;
        Node *prev;
        T value;
    };
private:
    Allocator *mAllocator{nullptr};
    Node *mHead{nullptr};
    Node *mTail{nullptr};
    int mLength{0};

public:
    explicit inline DoublyLinkedList(Allocator *a) : mAllocator(a) {
        printf("sizeof: : %zu\n", sizeof(Node));

        mHead = (Node *) mAllocator->Alloc(sizeof(Node));
        mTail = (Node *) mAllocator->Alloc(sizeof(Node));
        mHead->prev = nullptr;
        mHead->next = mTail;
        mTail->prev = mHead;
        mTail->next = nullptr;
    }

    explicit inline DoublyLinkedList(const DoublyLinkedList &) = delete;

    inline ~DoublyLinkedList() {
        clear();
        mAllocator->Free((void **) &mHead);
        mAllocator->Free((void **) &mTail);
    }

    inline void insertAfter(Node *node, const T &value) {
        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
        newNode->value = value;
        Node *next = node->next;
        newNode->prev = node;
        newNode->next = next;
        node->next = newNode;
        next->prev = newNode;
        mLength++;

    }

    inline void insertBefore(Node *node, const T &value) {
        Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
        newNode->value = value;
        Node *prev = node->prev;
        newNode->next = node;
        newNode->prev = prev;
        node->prev = newNode;
        prev->next = newNode;
        mLength++;
    }

    inline void remove(Node *node) {
        assert(node->next != nullptr && node->prev != nullptr &&
               "DoublyLinkedList: node not exist");

        Node *prev = node->prev;
        Node *next = node->next;
        prev->next = node->next;
        next->prev = node->prev;
        mAllocator->Free((void **) &node);
        mLength--;

    }

    inline void remove(const T &value) {
        Node *it = mHead->next;
        while (it != mTail) {
            if (it->value == value) break;
            it = it->next;
        }
        remove(it);
    }

    inline bool search(const T &value) {
        Node *it = mHead->next;
        while (it != mTail) {
            if (it->value == value) return true;
            it = it->next;
        }
        return false;
    }

    inline void pushFront(const T &value) {
        insertAfter(mHead, value);
    }

    inline void pushBack(const T &value) {

        insertBefore(mTail, value);
    }

    inline T popFront() {
        assert(mHead->next != nullptr && mHead->next != mTail &&
               "DoublyLinkedList: is empty");
        T value = mHead->next->value;
        remove(mHead->next);
        return value;
    }

    inline T popBack() {
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


    inline const int &size() {
        return mLength;
    }

    inline Node *head() const {
        return mHead->next;
    }

    inline Node *tail() const {
        return mTail;
    }

    inline void clear() {

        Node *it = mHead->next;
        while (it != mTail) {
            Node *tmp = it;
            it = it->next;
            mAllocator->Free((void **) &tmp);
        }
        mHead->next = mTail;
        mTail->prev = mHead;
        mLength = 0;

    }


};