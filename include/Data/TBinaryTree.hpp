#pragma once

#include "data/TArray.hpp"
#include "cmath"
#include "engine/Memory.hpp"

template<typename T, class TAlloc>
class TBinaryTree {
private:
    struct Node {
        T value;
        Node *left;
        Node *right;
    };

public:
    Node *mRoot{nullptr};
    int mLength{0};
public:

    explicit inline TBinaryTree() {}

    explicit inline TBinaryTree(const TBinaryTree &) = delete;

    inline void Insert(T value) {
        mRoot = insert(mRoot, value);
    }

    inline const T &Min() {
        return min(mRoot);
    }

    inline const T &Max() {
        return max(mRoot);
    }

    inline bool Validate() {
        return validate(mRoot);
    }

    inline bool Search(T value) {
        return search(mRoot, value);
    }

    inline const int &Length() {
        return mLength;
    }

    inline void Remove(T value) {
        mRoot = remove(mRoot, value);
    }

private:

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

    inline Node *insert(Node *node, T value) {
        if (node == nullptr) {
            Node *newNode = Alloc<TAlloc, Node>();
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


    inline const T &min(Node *root) {
        assert(root != nullptr && "BST: is empty");
        while (root->left != nullptr) {
            root = root->left;
        }
        return root->value;
    }


    inline const T &max(Node *root) {
        assert(root != nullptr && "BST: is empty");
        while (root->right != nullptr) {
            root = root->right;
        }
        return root->value;
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

    inline bool search(Node *node, T value) {
        if (node == nullptr)
            return false;
        if (value > node->value)
            return search(node->right, value);

        if (value < node->value)
            return search(node->left, value);

        return true;
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
            Free<TAlloc>(&node);
            mLength--;
            return right;
        }

        if (node->right == nullptr) {
            Node *left = node->left;
            Free<TAlloc>(&node);
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
        Free<TAlloc>(&successor);
        mLength--;

        return node;
    }

};

