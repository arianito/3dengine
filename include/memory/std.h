#pragma once

#include <stddef.h>

void *std_alloc(size_t size, int alignment);
void std_free(void **ptr);
