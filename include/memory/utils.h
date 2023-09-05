#pragma once

#include <stddef.h>

inline unsigned int calculate_padding(size_t address, unsigned int alignment)
{
	unsigned int modulo = address & (alignment - 1);

	if (modulo == 0)
		return 0;

	return alignment - modulo;
}

inline unsigned int calculate_space(unsigned int space, unsigned int alignment)
{
	unsigned int padding = alignment * (space / alignment);
	unsigned int modulo = space & (alignment - 1);

	if (modulo == 0)
		return padding;

	return padding + alignment;
}

inline unsigned int calculate_alignment(size_t address, unsigned int space, unsigned int alignment)
{
	unsigned int padding = calculate_padding(address, alignment);
	if (padding >= space)
		return padding;
	return padding + calculate_space(space, alignment);
}

inline void byte7a(size_t *f, size_t a, unsigned char b)
{
	size_t m = (8 * (sizeof(a) - sizeof(b)));
	*f = ((size_t)b << m) | (a & (((size_t)1 << m) - 1));
}

inline void byte7d(size_t f, size_t *a, unsigned char *b)
{
	size_t m = (8 * (sizeof(*a) - sizeof(*b)));
	if (a != NULL)
		*a = (f & (((size_t)1 << m) - 1));
	if (b != NULL)
		*b = (f >> m) & (((size_t)1 << (sizeof(*b) * 8)) - 1);
}

inline void byte7a7(size_t *f, size_t a)
{
	size_t mask = (((size_t)1 << (8 * (sizeof(a) - sizeof(char)))) - 1);
	*f = (~mask & *f) | (mask & a);
}

inline void byte7a1(size_t *f, unsigned char b)
{
	size_t m = (8 * (sizeof(*f) - sizeof(b)));
	*f = ((size_t)b << m) | ((((size_t)1 << m) - 1) & *f);
}

inline void byte6a(size_t *f, size_t a, unsigned short b)
{
	size_t m = (8 * (sizeof(a) - sizeof(b)));
	*f = ((size_t)b << m) | (a & (((size_t)1 << m) - 1));
}

inline void byte6d(size_t f, size_t *a, unsigned short *b)
{
	size_t m = (8 * (sizeof(*a) - sizeof(*b)));
	if (a != NULL)
		*a = (f & (((size_t)1 << m) - 1));
	if (b != NULL)
		*b = (f >> m) & (((size_t)1 << (sizeof(*b) * 8)) - 1);
}

inline void byte6a6(size_t *f, size_t a)
{
	size_t mask = (((size_t)1 << (8 * (sizeof(a) - sizeof(short)))) - 1);
	*f = (~mask & *f) | (mask & a);
}

inline void byte6a2(size_t *f, unsigned short b)
{
	size_t m = (8 * (sizeof(*f) - sizeof(b)));
	*f = ((size_t)b << m) | ((((size_t)1 << m) - 1) & *f);
}