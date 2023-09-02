#pragma once

#include "stddef.h"

typedef struct
{
    size_t length;
    char *text;
} File;

void file_init(char* p);
File *file_read(const char *p);
void file_destroy(File *f);