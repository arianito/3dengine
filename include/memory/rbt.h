#pragma once
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

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	RED,
	BLACK
} RedBlackTreeColorEnum;

typedef struct
{
	int data;
	RedBlackTreeColorEnum color;
	void *parent;
	void *left;
	void *right;
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