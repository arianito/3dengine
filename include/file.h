#pragma once

#include <stddef.h>

#define URL_LENGTH 512

typedef struct
{
    size_t length;
    char *text;
} File;

typedef struct
{
    char prefix[128];

} FileData;

FileData *file;

void resolve(const char *p, char out[URL_LENGTH]);
void file_init(const char *pfx);
File *file_read(const char *p);
void file_destroy(File *f);