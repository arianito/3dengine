
#include "mem/std.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

void *std_alloc(unsigned int size, unsigned int alignment) {
    if (!ISPOW2(alignment)) {
        printf("std: alloc failed, invalid alignment\n");
        return NULL;
    }
    void *ptr = malloc(size + alignment);
    if (ptr == NULL) {
        printf("std: system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }

    size_t address = (size_t) ptr;
    unsigned int padding = MEMORY_ALIGNMENT(address, 1, alignment);
    address += padding;
    *((char *) (address - 1)) = padding;
    return (void *) address;
}

unsigned char std_free(void **ptr) {
    if (ptr == NULL || (*ptr) == NULL) {
        printf("std: free failed, invalid pointer\n");
        return 0;
    }
    size_t address = (size_t) (*ptr);
    free((void *) (address - *((char *) (address - 1))));
    *ptr = NULL;
    return 1;
}