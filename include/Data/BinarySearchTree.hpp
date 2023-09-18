#pragma once

#include "data/Array.hpp"
#include "cmath"
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

template<typename T>
class BinarySearchTree {
public:
    struct Node {
        T value;
        Node *left;
        Node *right;
    };

public:
    Node *mRoot{nullptr};
    Allocator *mAllocator{nullptr};
    int mLength{0};
public:

    explicit inline BinarySearchTree(Allocator *a) : mAllocator(a) {}

    explicit inline BinarySearchTree(const BinarySearchTree &) = delete;

    inline Node *insert(Node *node, T value) {
        if (node == nullptr) {
            Node *newNode = (Node *) mAllocator->Alloc(sizeof(Node));
            newNode->left = nullptr;
            newNode->right = nullptr;
            newNode->value = value;
            mLength++;
            return newNode;
        }

        if (value > node->value) {
            node->right = insert(node->right, value);
        } else if (value < node->value) {
            node->left = insert(node->left, value);
        }

        return node;
    }

    inline int height(Node *node) {
        if (node == nullptr)
            return 0;
        return 1 + (int) fmaxf(height(node->left), height(node->right));
    }

    inline int size(Node *node) {
        if (node == nullptr)
            return 0;
        return 1 + size(node->left) + size(node->right);
    }

    inline float width(Node *root) {
        if (root == nullptr) return 0;
        return fmaxf(1, width(root->left) + width(root->right));
    }

    inline void insert(T value) {
        mRoot = insert(mRoot, value);
    }


    inline const T &min(Node *root) {
        assert(root != nullptr && "BST: is empty");
        while (root->left != nullptr) {
            root = root->left;
        }
        return root->value;
    }

    inline const T &min() {
        return min(mRoot);
    }

    inline const T &max(Node *root) {
        assert(root != nullptr && "BST: is empty");
        while (root->right != nullptr) {
            root = root->right;
        }
        return root->value;
    }

    inline const T &max() {
        return max(mRoot);
    }

    inline bool isLesser(Node *root, T value) {
        if(root == nullptr)
            return true;
        if(root->value > value)
            return false;
        return isLesser(root->left, value) && isLesser(root->right, value);
    }

    inline bool isGreater(Node *root, T value) {
        if(root == nullptr)
            return true;
        if(root->value < value)
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
    
    inline bool validate() {
        return validate(mRoot);
    }


    inline bool search(Node *node, T value) {
        if (node == nullptr)
            return false;
        if (value > node->value)
            return search(node->right, value);

        if (value < node->value)
            return search(node->left, value);

        return true;
    }

    inline bool search(T value) {
        return search(mRoot, value);
    }

    inline Node *remove(Node *node, T value) {
        if (node == nullptr)
            return node;

        if (value > node->value) {
            node->right = remove(node->right, value);
            return node;
        }

        if (value < node->value) {
            node->left = remove(node->left, value);
            return node;
        }

        if (node->left == nullptr) {
            Node *right = node->right;
            mAllocator->Free((void **) &node);
            mLength--;
            return right;
        }

        if (node->right == nullptr) {
            Node *left = node->left;
            mAllocator->Free((void **) &node);
            mLength--;
            return left;
        }

        Node *successor = node->right, *parent = node;
        while (successor->left != nullptr) {
            parent = successor;
            successor = successor->left;
        }

        if (parent == node) {
            node->right = successor->right;
        } else {
            parent->left = successor->right;
        }

        node->value = successor->value;
        mAllocator->Free((void **) &successor);
        mLength--;

        return node;
    }

    inline void remove(T value) {
        mRoot = remove(mRoot, value);
    }
};

