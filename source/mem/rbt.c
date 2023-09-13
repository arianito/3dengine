/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/
#include "mem/rbt.h"

#include <stdio.h>
#include <stdlib.h>

RedBlackTreeNode *createNode(int data) {
    RedBlackTreeNode *newNode = (RedBlackTreeNode *) malloc(sizeof(RedBlackTreeNode));
    newNode->data = data;
    newNode->color = RED;
    newNode->parent = newNode->left = newNode->right = NULL;
    return newNode;
}

void leftRotate(RedBlackTreeNode **root, RedBlackTreeNode *x) {
    RedBlackTreeNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        ((RedBlackTreeNode *) y->left)->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        *root = y;
    else if (x == ((RedBlackTreeNode *) x->parent)->left)
        ((RedBlackTreeNode *) x->parent)->left = y;
    else
        ((RedBlackTreeNode *) x->parent)->right = y;
    y->left = x;
    x->parent = y;
}

void rightRotate(RedBlackTreeNode **root, RedBlackTreeNode *y) {
    RedBlackTreeNode *x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        ((RedBlackTreeNode *) x->right)->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL)
        *root = x;
    else if (y == ((RedBlackTreeNode *) y->parent)->left)
        ((RedBlackTreeNode *) y->parent)->left = x;
    else
        ((RedBlackTreeNode *) y->parent)->right = x;
    x->right = y;
    y->parent = x;
}

void fixInsertion(RedBlackTreeNode **root, RedBlackTreeNode *z) {
    while (z != *root && ((RedBlackTreeNode *) z->parent)->color == RED) {
        if (z->parent == ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->left) {
            RedBlackTreeNode *y = ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->right;
            if (y != NULL && y->color == RED) {
                ((RedBlackTreeNode *) z->parent)->color = BLACK;
                y->color = BLACK;
                ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->color = RED;
                z = ((RedBlackTreeNode *) z->parent)->parent;
            } else {
                if (z == ((RedBlackTreeNode *) z->parent)->right) {
                    z = z->parent;
                    leftRotate(root, z);
                }
                ((RedBlackTreeNode *) z->parent)->color = BLACK;
                ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->color = RED;
                rightRotate(root, ((RedBlackTreeNode *) z->parent)->parent);
            }
        } else {
            RedBlackTreeNode *y = ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->left;
            if (y != NULL && y->color == RED) {
                ((RedBlackTreeNode *) z->parent)->color = BLACK;
                y->color = BLACK;
                ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->color = RED;
                z = ((RedBlackTreeNode *) z->parent)->parent;
            } else {
                if (z == ((RedBlackTreeNode *) z->parent)->left) {
                    z = z->parent;
                    rightRotate(root, z);
                }
                ((RedBlackTreeNode *) z->parent)->color = BLACK;
                ((RedBlackTreeNode *) ((RedBlackTreeNode *) z->parent)->parent)->color = RED;
                leftRotate(root, ((RedBlackTreeNode *) z->parent)->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

void fixDeletion(RedBlackTreeNode **root, RedBlackTreeNode *x, RedBlackTreeNode *xParent) {
    while (x != *root && (x == NULL || x->color == BLACK)) {
        if (x == xParent->left) {
            RedBlackTreeNode *w = xParent->right;
            if (w->color == RED) {
                w->color = BLACK;
                xParent->color = RED;
                leftRotate(root, xParent);
                w = xParent->right;
            }
            if ((w->left == NULL || ((RedBlackTreeNode *) w->left)->color == BLACK) &&
                (w->right == NULL || ((RedBlackTreeNode *) w->right)->color == BLACK)) {
                w->color = RED;
                x = xParent;
                xParent = xParent->parent;
            } else {
                if (w->right == NULL || ((RedBlackTreeNode *) w->right)->color == BLACK) {
                    if (w->left != NULL)
                        ((RedBlackTreeNode *) w->left)->color = BLACK;
                    w->color = RED;
                    rightRotate(root, w);
                    w = xParent->right;
                }
                w->color = xParent->color;
                xParent->color = BLACK;
                if (w->right != NULL)
                    ((RedBlackTreeNode *) w->right)->color = BLACK;
                leftRotate(root, xParent);
                x = *root;
            }
        } else {
            RedBlackTreeNode *w = xParent->left;
            if (w->color == RED) {
                w->color = BLACK;
                xParent->color = RED;
                rightRotate(root, xParent);
                w = xParent->left;
            }
            if ((w->right == NULL || ((RedBlackTreeNode *) w->right)->color == BLACK) &&
                (w->left == NULL || ((RedBlackTreeNode *) w->left)->color == BLACK)) {
                w->color = RED;
                x = xParent;
                xParent = xParent->parent;
            } else {
                if (w->left == NULL || ((RedBlackTreeNode *) w->left)->color == BLACK) {
                    if (w->right != NULL)
                        ((RedBlackTreeNode *) w->right)->color = BLACK;
                    w->color = RED;
                    leftRotate(root, w);
                    w = xParent->left;
                }
                w->color = xParent->color;
                xParent->color = BLACK;
                if (w->left != NULL)
                    ((RedBlackTreeNode *) w->left)->color = BLACK;
                rightRotate(root, xParent);
                x = *root;
            }
        }
    }
    if (x != NULL)
        x->color = BLACK;
}

RedBlackTreeNode *findMin(RedBlackTreeNode *root) {
    while (root->left != NULL)
        root = root->left;
    return root;
}

RedBlackTreeNode *findMax(RedBlackTreeNode *root) {
    while (root->right != NULL)
        root = root->right;
    return root;
}

RedBlackTreeNode *search(RedBlackTreeNode *root, int data) {
    if (root == NULL || root->data == data)
        return root;
    if (data < root->data)
        return search(root->left, data);
    return search(root->right, data);
}

void transplant(RedBlackTreeNode **root, RedBlackTreeNode *u, RedBlackTreeNode *v) {
    if (u->parent == NULL)
        *root = v;
    else if (u == ((RedBlackTreeNode *) u->parent)->left)
        ((RedBlackTreeNode *) u->parent)->left = v;
    else
        ((RedBlackTreeNode *) u->parent)->right = v;
    if (v != NULL)
        v->parent = u->parent;
}

void deleteNode(RedBlackTreeNode **root, RedBlackTreeNode *z) {
    RedBlackTreeNode *y = z;
    RedBlackTreeNode *x;
    RedBlackTreeColorEnum yOriginalColor = y->color;

    if (z->left == NULL) {
        x = z->right;
        transplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        transplant(root, z, z->left);
    } else {
        y = findMin(z->right);
        yOriginalColor = y->color;
        x = y->right;

        if (y->parent == z)
            x->parent = y;
        else {
            transplant(root, y, y->right);
            y->right = z->right;
            ((RedBlackTreeNode *) y->right)->parent = y;
        }

        transplant(root, z, y);
        y->left = z->left;
        ((RedBlackTreeNode *) y->left)->parent = y;
        y->color = z->color;
    }

    if (yOriginalColor == BLACK)
        fixDeletion(root, x, x->parent);

    free(z);
}

void insert(RedBlackTreeNode **root, int data) {
    RedBlackTreeNode *z = createNode(data);
    RedBlackTreeNode *y = NULL;
    RedBlackTreeNode *x = *root;

    while (x != NULL) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if (y == NULL)
        *root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    fixInsertion(root, z);
}

void inOrderTraversal(RedBlackTreeNode *root) {
    if (root == NULL)
        return;

    inOrderTraversal(root->left);
    printf("%d,", root->data);
    inOrderTraversal(root->right);
}

void freeTree(RedBlackTreeNode *root) {
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
