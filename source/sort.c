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
#include "sort.h"

#include <stdlib.h>
#include <time.h>

void swap(size_t *a, size_t *b)
{
	if (a == b)
		return;
	size_t tmp = *a;
	*a = *b;
	*b = tmp;
}

void sort_insert(size_t arr[], int a, int b)
{
	int i = a + 1, j;
	size_t key;
	for (; i <= b; i++)
	{
		j = i - 1;
		key = arr[i];
		for (; j >= a && key < arr[j]; j--)
			swap(&arr[j + 1], &arr[j]);
		arr[j + 1] = key;
	}
}

void sort_quick(size_t arr[], int a, int b)
{
	if (a >= b)
		return;
		
	int m = a + (b - a) / 2;
	if (arr[a] > arr[m])
		swap(&arr[a], &arr[m]);

	if (arr[a] > arr[b])
		swap(&arr[a], &arr[b]);

	if (arr[b] > arr[m])
		swap(&arr[b], &arr[m]);

	swap(&arr[m], &arr[b]);

	size_t key = arr[b];
	int i = a;
	int j = a - 1;
	for (; i < b; i++)
		if (key > arr[i])
			swap(&arr[i], &arr[++j]);
	swap(&arr[b], &arr[++j]);

	sort_quick(arr, a, j - 1);
	sort_quick(arr, j + 1, b);
}

void merge(StackMemory *stack, size_t arr[], int a, int m, int b)
{
	int n1 = m - a + 1;
	int n2 = b - m;

	size_t *L = (size_t *)stack_alloc(stack, n1 * sizeof(size_t), 8);
	size_t *R = (size_t *)stack_alloc(stack, n2 * sizeof(size_t), 8);

	for (int i = 0; i < n1; i++)
		L[i] = arr[a + i];

	for (int i = 0; i < n2; i++)
		R[i] = arr[m + i + 1];

	int i = 0, j = 0, k = a;

	while (i < n1 && j < n2)
		if (L[i] < R[j])
			arr[k++] = L[i++];
		else
			arr[k++] = R[j++];

	while (i < n1)
		arr[k++] = L[i++];
	while (j < n2)
		arr[k++] = R[j++];

	stack_free(stack, &R);
	stack_free(stack, &L);
}

void sort_merge(StackMemory *stack, size_t arr[], int a, int b)
{
	if (a >= b)
		return;

	int mid = a + (b - a) / 2;

	sort_merge(stack, arr, a, mid);
	sort_merge(stack, arr, mid + 1, b);

	merge(stack, arr, a, mid, b);
}

void sort_shuffle(size_t arr[], int a, int b)
{
	if (a >= b)
		return;

	srand((int)time(NULL));
	for (int i = b; i > a; i--)
	{
		int j = a + rand() % (i - a + 1);
		swap(&arr[i], &arr[j]);
	}
}