#pragma once

#include <stddef.h>

inline int calculate_padding(size_t address, int alignment)
{
	int modulo = address & (alignment - 1);

	if (modulo == 0)
		return 0;

	return alignment - modulo;
}

inline int calculate_space(int space, int alignment)
{
	int padding = alignment * (space / alignment);
	int modulo = space & (alignment - 1);

	if (modulo == 0)
		return padding;

	return padding + alignment;
}

inline int calculate_alignment(size_t address, int space, int alignment)
{
	int padding = calculate_padding(address, alignment);
	if (padding >= space)
		return padding;
	return padding + calculate_space(space, alignment);
}

inline size_t byte6a(size_t *f, size_t a, short b)
{
	size_t m = (8 * (sizeof(a) - sizeof(b)));
	*f = ((size_t)b << m) | (a & (((size_t)1 << m) - 1));
}

inline void byte6d(size_t f, size_t *a, short *b)
{
	size_t m = (8 * (sizeof(*a) - sizeof(*b)));
	*a = (f & (((size_t)1 << m) - 1));
	*b = (f >> m) & (((size_t)1 << (sizeof(b) * 8)) - 1);
}

inline size_t byte7a(size_t *f, size_t a, char b)
{
	size_t m = (8 * (sizeof(a) - sizeof(b)));
	*f = ((size_t)b << m) | (a & (((size_t)1 << m) - 1));
}

inline void byte7d(size_t f, size_t *a, char *b)
{
	size_t m = (8 * (sizeof(*a) - sizeof(*b)));
	*a = (f & (((size_t)1 << m) - 1));
	*b = (f >> m) & (((size_t)1 << (sizeof(b) * 8)) - 1);
}