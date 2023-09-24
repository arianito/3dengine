#pragma once


#include <stddef.h>

void *std_alloc(unsigned int size, unsigned int alignment);

unsigned char std_free(void **ptr);
