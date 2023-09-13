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
#include "mem/std.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mem/utils.h"

void *std_alloc(size_t size, unsigned int alignment)
{
	if (!ISPOW2(alignment))
	{
		printf("std: alloc failed, invalid alignment\n");
		return NULL;
	}
	void *ptr = malloc(size + alignment);
	if (ptr == NULL)
	{
		printf("std: system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}

	size_t address = (size_t)ptr;
	unsigned int padding = MEMORY_ALIGNMENT(address, 1, alignment);
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