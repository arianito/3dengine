#pragma once


#include <stddef.h>
#include <stdio.h>

#define BYTES (1)
#define KILOBYTES (BYTES * 1024)
#define MEGABYTES (KILOBYTES * 1024)
#define GIGABYTES (MEGABYTES * 1024)

void format_bytes(double bytes, char *buff, unsigned int n);

void clear(void *p, size_t s);