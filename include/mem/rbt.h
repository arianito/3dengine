#pragma once


#include <stdio.h>
#include <stdlib.h>

typedef enum {
    RED,
    BLACK
} RedBlackTreeColorEnum;

typedef struct {
    void *parent;
    void *left;
    void *right;
    RedBlackTreeColorEnum color;
    int data;
} RedBlackTreeNode;

RedBlackTreeNode *createNode(int data);

void leftRotate(RedBlackTreeNode **root, RedBlackTreeNode *x);

void rightRotate(RedBlackTreeNode **root, RedBlackTreeNode *y);

void fixInsertion(RedBlackTreeNode **root, RedBlackTreeNode *z);

void fixDeletion(RedBlackTreeNode **root, RedBlackTreeNode *x, RedBlackTreeNode *xParent);

RedBlackTreeNode *findMin(RedBlackTreeNode *root);

RedBlackTreeNode *findMax(RedBlackTreeNode *root);

RedBlackTreeNode *search(RedBlackTreeNode *root, int data);

void transplant(RedBlackTreeNode **root, RedBlackTreeNode *u, RedBlackTreeNode *v);

void deleteNode(RedBlackTreeNode **root, RedBlackTreeNode *z);

void insert(RedBlackTreeNode **root, int data);

void inOrderTraversal(RedBlackTreeNode *root);

void freeTree(RedBlackTreeNode *root);