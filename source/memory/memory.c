#include "memory/memory.h"

#include <stdio.h>
#include <string.h>

void format_bytes(double bytes, char *buff, size_t n)
{
	const char suffix[5][3] = {"B\0", "KB\0", "MB\0", "GB\0", "TB\0"};
	int i = 0;

	while (bytes >= 1000 && i < 5)
	{
		bytes /= 1000;
		i++;
	}

	snprintf(buff, n, "%.2f %s", bytes, suffix[i]);
}

void clear(void *p, size_t s)
{
	memset(p, 0, s);
}