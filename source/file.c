
#include "file.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "alloc.h"

static char *prefix;

void file_init(char *p)
{
    prefix = p;
}

File *file_read(const char *p)
{
    char b[255];
    sprintf_s(b, 255, "%s%s", prefix, p);

    FILE *file;
    fopen_s(&file, b, "r");
    assert(file != NULL && "failed to open the file");

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

        File *buff = (File *)alloc_stack(sizeof(File));
    buff->length = file_size;

    buff->text = (char *)alloc_stack(file_size + 1);
    size_t bytes_read = fread(buff->text, 1, file_size, file);
    buff->text[bytes_read] = '\0';
    fclose(file);
    return buff;
}

void file_destroy(File *f)
{
    free_stack(f->text);
    free_stack(f);
}