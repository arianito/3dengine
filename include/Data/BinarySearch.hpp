#pragma

#include <utility>

template<typename T>
void swap(T &a, T &b) {
    if (a == b)
        return;
    std::swap(a, b);
}


template<typename T>
inline void SortUp(T arr[], int a, int b) {
    if (b < a)
        return;

    int i = a;
    int j = a - 1;
    T &key = arr[b];
    for (; i < b; i++)
        if (key > arr[i])
            swap<T>(arr[i], arr[++j]);
    swap<T>(arr[b], arr[++j]);

    SortUp<T>(arr, a, j - 1);
    SortUp<T>(arr, j + 1, b);
}

template<typename T>
inline int
BinarySearch(T searchItem, T arr[], int a, int b) {
    if (b < a)
        return -1;

    int midpoint = a + (b - a) / 2;

    if (arr[midpoint] == searchItem)
        return midpoint;

    if (searchItem > arr[midpoint])
        return BinarySearch(searchItem, arr, midpoint + 1, b);

    if (searchItem < arr[midpoint])
        return BinarySearch(searchItem, arr, a, midpoint - 1);

    return -1;
}