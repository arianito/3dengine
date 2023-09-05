#pragma once

#include <stddef.h>

void *std_alloc(size_t size, unsigned int alignment);
unsigned char std_free(void **ptr);
