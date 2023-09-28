#pragma once

#include "engine/Memory.hpp"

template<class T, class TAlloc>
class TAVLTree {
public:
    struct Node {
        T value;
        Node *left{nullptr};
        Node *right{nullptr};
        int height{1};

        explicit inline Node(const T &value) : value(value) {}

        explicit inline Node(const Node &value) = delete;
    };

    int mLength{};
    Node *mHead{};

public:
    explicit inline TAVLTree() = default;

    explicit inline TAVLTree(const TAVLTree &) = delete;

    inline ~TAVLTree() {
        destroy(mHead);
    }

    template<typename... Params>
    inline void Add(Params... params) {
        for (auto p: {params...}) {
            mHead = add(mHead, p);
        }
    }

    inline void Remove(const T &value) {
        mHead = remove(mHead, value);
    }

    inline const T &Front() {
        assert(mHead != nullptr && "AVLTree: is empty");
        return mHead->value;
    }

    inline bool Empty() {
        return mHead == nullptr;
    }

    inline int Length() {
        return mLength;
    }


public:

    inline Node *remove(Node *A, const T &value) {
        if (A == nullptr)
            return A;

        if (value > A->value) {
            A->right = remove(A->right, value);
            A->height = height(A);
        } else if (value < A->value) {
            A->left = remove(A->left, value);
            A->height = height(A);
        } else if (A->left == nullptr) {
            Node *right = A->right;
            if (right != nullptr) right->height = height(right);
            Free<TAlloc>(&A);
            mLength--;
            A = right;
        } else if (A->right == nullptr) {
            Node *left = A->left;
            if (left != nullptr) left->height = height(left);
            Free<TAlloc>(&A);
            mLength--;
            A = left;
        } else {
            Node *successor = A->right, *parent = A;
            while (successor->left != nullptr) {
                parent = successor;
                successor = successor->left;
            }
            if (parent == A) {
                parent->right = successor->right;
            } else {
                parent->left = successor->right;
            }

            parent->height = height(parent);
            A->height = height(A);
            A->value = successor->value;
            Free<TAlloc>(&successor);
            mLength--;
        }
        return balance(A);
    }

    inline Node *add(Node *A, const T &newValue) {
        if (A == nullptr) {
            mLength++;
            return AllocNew<TAlloc, Node>(newValue);
        }
        if (newValue > A->value) {
            A->right = add(A->right, newValue);
        } else if (newValue < A->value) {
            A->left = add(A->left, newValue);
        }
        A->height = height(A);
        return balance(A);
    }

    inline Node *balance(Node *A) {
        if (balanceFactor(A) == 2 && balanceFactor(A->left) == 1) {
            return rotateRight(A);
        } else if (balanceFactor(A) == 2 && balanceFactor(A->left) == -1) {
            return rotateLeftThenRight(A);
        } else if (balanceFactor(A) == -2 && balanceFactor(A->right) == 1) {
            return rotateRightThenLeft(A);
        } else if (balanceFactor(A) == -2 && balanceFactor(A->right) == -1) {
            return rotateLeft(A);
        }
        return A;
    }

    inline int balanceFactor(Node *node) {
        if (node == nullptr)
            return 0;
        int a = node->left ? node->left->height : 0;
        int b = node->right ? node->right->height : 0;
        return a - b;
    }

    inline int height(Node *node) {
        if (node == nullptr)
            return 0;
        int a = node->left ? node->left->height : 0;
        int b = node->right ? node->right->height : 0;
        return 1 + (a > b ? a : b);
    }

    inline int height2(Node *node) {
        if (node == nullptr)
            return 0;
        int a = height2(node->left);
        int b = height2(node->right);
        return 1 + (a > b ? a : b);
    }


    inline int size(Node *node) {
        if (node == nullptr) return 0;
        return 1 + size(node->left) + size(node->right);
    }

    inline Node *rotateRight(Node *A) {
        Node *B = A->left;

        Node *Br = B->right;
        B->right = A;
        A->left = Br;

        A->height = height(A);
        B->height = height(B);

        return B;
    }

    inline Node *rotateLeft(Node *A) {
        Node *B = A->right;

        Node *Bl = B->left;
        B->left = A;
        A->right = Bl;

        A->height = height(A);
        B->height = height(B);

        return B;
    }

    inline Node *rotateLeftThenRight(Node *A) {
        Node *B = A->left;
        Node *C = B->right;

        A->left = C->right;
        B->right = C->left;
        C->left = B;
        C->right = A;


        B->height = height(B);
        A->height = height(A);
        C->height = height(C);

        return C;
    }


    inline Node *rotateRightThenLeft(Node *A) {
        Node *B = A->right;
        Node *C = B->left;

        A->right = C->left;
        B->left = C->right;
        C->left = A;
        C->right = B;

        B->height = height(B);
        A->height = height(A);
        C->height = height(C);

        return C;
    }


    void destroy(Node *node) {
        if (node == nullptr)
            return;
        Node *left = node->left;
        Node *right = node->right;
        Free<TAlloc>(&node);
        destroy(left);
        destroy(right);
    }


    inline bool isLesser(Node *root, T value) {
        if (root == nullptr)
            return true;
        if (root->value > value)
            return false;
        return isLesser(root->left, value) && isLesser(root->right, value);
    }

    inline bool isGreater(Node *root, T value) {
        if (root == nullptr)
            return true;
        if (root->value < value)
            return false;
        return isGreater(root->left, value) && isGreater(root->right, value);
    }

    inline bool validate(Node *root) {
        if (root == nullptr)
            return true;

        return isLesser(root->left, root->value) &&
               isGreater(root->right, root->value) &&
               validate(root->left) &&
               validate(root->right);

    }

    inline void inorder(Node *root) {

        if (root == nullptr)
            return;

        inorder(root->left);
        printf("%d\n ", root->value);
        inorder(root->right);
    }
};