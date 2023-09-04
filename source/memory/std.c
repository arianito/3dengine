#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/std.h"
#include "memory/utils.h"

void *std_alloc(size_t size, int alignment)
{
	assert((alignment & (alignment - 1)) == 0 && "invalid alignment");
	void *ptr = malloc(size + alignment);
	if (ptr == NULL)
	{
		printf("std: system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}

	size_t address = (size_t)ptr;
	int padding = calculate_alignment(address, 1, alignment);
	address += padding;
	*((char *)(address - 1)) = padding;
	return (void *)address;
}

void std_free(void **ptr)
{
	if (ptr == NULL || *ptr == NULL)
		return;
	size_t address = (size_t)(*ptr);
	free((void *)(address - *((char *)(address - 1))));
}