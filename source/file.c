
#include "file.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

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

    char *buffer = (char *)malloc(file_size + 1);
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);

    File *buff = (File *)malloc(sizeof(File));
    buff->length = file_size;
    buff->text = buffer;
    return buff;
}

void file_destroy(File *f)
{
    free(f->text);
    free(f);
}