#pragma once

#include <stddef.h>
#include "memory/stack.h"

void sort_insert(size_t arr[], int a, int b);
void sort_quick(size_t arr[], int a, int b);
void sort_merge(StackMemory *stack, size_t arr[], int a, int b);
void sort_shuffle(size_t arr[], int a, int b);