

#include "file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mem/alloc.h"

static char *prefix;
static int prefixLength;

char *resolve(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    char *out = alloc_stack(char, prefixLength + len + 1);

    char *buffer = alloc_stack(char, len + 1);
    if (prefix != NULL)
        vsnprintf(buffer, len + 1, fmt, args);
    va_end(args);

    sprintf(out, "%s%s", prefix, buffer);
    alloc_free((void **) &buffer);

    return out;
}

void file_init(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    prefixLength = len;
    prefix = alloc_global(char, len + 1);
    if (prefix != NULL)
        vsnprintf(prefix, len + 1, fmt, args);
    va_end(args);
}

File *file_read(const char *p) {
    char *path = resolve(p);

    FILE *f;
    fopen_s(&f, path, "r");

    alloc_free((void **) &path);

    if (f == NULL) {
        perror("failed to open the file");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    File *buff = alloc_stack(File, sizeof(File) + file_size + 1);
    buff->length = file_size;
    buff->text = (char *) ((size_t) buff + sizeof(File));
    size_t bytes_read = fread(buff->text, 1, file_size, f);
    buff->text[bytes_read] = '\0';
    fclose(f);
    return buff;
}

void file_destroy(File **f) {
    alloc_free((void **) f);
    *f = NULL;
}