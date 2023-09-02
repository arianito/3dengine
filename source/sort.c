#include "sort.h"
#include "mem.h"

void swap(int *a, int *b)
{
    if (a == b)
        return;
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_insert(int arr[], int a, int b)
{
    int i = a + 1, j, key;
    for (; i <= b; i++)
    {
        j = i - 1;
        key = arr[i];
        for (; j >= a && key < arr[j]; j--)
            swap(&arr[j + 1], &arr[j]);
        arr[j + 1] = key;
    }
}

void sort_quick(int arr[], int a, int b)
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

    int key = arr[b];
    int i = a;
    int j = a - 1;
    for (; i < b; i++)
        if (key > arr[i])
            swap(&arr[i], &arr[++j]);
    swap(&arr[b], &arr[++j]);

    sort_quick(arr, a, j - 1);
    sort_quick(arr, j + 1, b);
}

void merge(void *stack_mem, int arr[], int a, int m, int b)
{
    int n1 = m - a + 1;
    int n2 = b - m;

    int *L = (int *)mstack_alloc(stack_mem, n1 * sizeof(int), 8);
    int *R = (int *)mstack_alloc(stack_mem, n2 * sizeof(int), 8);

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

    mstack_free(stack_mem, R);
    mstack_free(stack_mem, L);
}

void sort_merge(void *stack_mem, int arr[], int a, int b)
{
    if (a >= b)
        return;

    int mid = a + (b - a) / 2;

    sort_merge(stack_mem, arr, a, mid);
    sort_merge(stack_mem, arr, mid + 1, b);

    merge(stack_mem, arr, a, mid, b);
}