#include "memory/std.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/utils.h"

void *std_alloc(size_t size, unsigned int alignment)
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
	unsigned int padding = calculate_alignment(address, 1, alignment);
	address += padding;
	*((char *)(address - 1)) = padding;
	return (void *)address;
}

unsigned char std_free(void **ptr)
{
	if (ptr == NULL || (*ptr) == NULL)
	{
		printf("std: free failed, invalid pointer\n");
		return 0;
	}
	size_t address = (size_t)(*ptr);
	free((void *)(address - *((char *)(address - 1))));
	*ptr = NULL;
	return 1;
}