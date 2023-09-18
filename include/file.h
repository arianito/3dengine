#pragma once


#include <stddef.h>

typedef struct {
    size_t length;
    char *text;
} File;

char *resolve(const char *fmt, ...);

void file_init(const char *fmt, ...);

File *file_read(const char *p);