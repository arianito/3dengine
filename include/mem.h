#pragma once

#include <stddef.h>

void *marena_create(size_t size, unsigned char alignment);
void marena_destroy(void *mem);
char marena_check(void *mem, size_t size, unsigned char alignment);
void *marena_alloc(void *mem, size_t size, unsigned char alignment);
void marena_free(void *mem, void *p);
void *marena_resize(void *mem, size_t size);
void marena_usage(void *mem);

void *mstack_create(size_t size, unsigned char alignment);
void mstack_destroy(void *mem);
char mstack_check(void *mem, size_t size, unsigned char alignment);
void *mstack_alloc(void *mem, size_t size, unsigned char alignment);
void mstack_free(void *mem, void *p);
void *mstack_resize(void *mem, size_t size);
void mstack_usage(void *mem);

void *mpool_create(size_t size, size_t chunk_size, unsigned char alignment);
void mpool_destroy(void *mem);
char mpool_check(void *mem);
void *mpool_alloc(void *mem);
void mpool_free(void *mem, void *p);
size_t mpool_space(void *mem);
void *mpool_resize(void *mem, size_t size);
void mpool_usage(void *mem);