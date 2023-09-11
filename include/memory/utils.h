#pragma once
/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/

#include <stddef.h>

#define MODULO(address, alignment) (address & (alignment - 1UL))
#define ISPOW2(alignment) (!(alignment & (alignment - 1UL)))
#define MEMORY_PADDING(address, alignment) ((alignment - (address & (alignment - 1UL))) & (alignment - 1UL))
#define MEMORY_PADDING_STD(address) (MEMORY_PADDING(address, sizeof(size_t)))
#define MEMORY_SPACE(space, alignment) ((space + alignment - 1UL) & ~(alignment - 1UL))
#define MEMORY_SPACE_STD(type) (MEMORY_SPACE(sizeof(type), sizeof(size_t)))
#define MEMORY_ALIGNMENT(address, space, alignment) (MEMORY_PADDING(address, alignment) + ((MEMORY_PADDING(address, alignment) >= space) ? 0UL : MEMORY_SPACE(space, alignment)))
#define MEMORY_ALIGNMENT_STD(address, type) (MEMORY_ALIGNMENT(address, sizeof(type), sizeof(size_t)))

#define BYTE_BE(offset) ((sizeof(1ULL) - (size_t)(offset + 1ULL)) << 3ULL)
#define BYTE_LE(offset) ((size_t)(offset) << 3ULL)
#define MASK_BE(offset) ((1ULL << BYTE_BE(offset - 1ULL)) - 1ULL)
#define MASK_LE(offset) ((1ULL << BYTE_LE(offset)) - 1ULL)
#define SET_NTH_BE(number, bytes, offset) (((number & MASK_LE(bytes)) << BYTE_BE(offset)))
#define SET_NTH_LE(number, bytes, offset) (((number & MASK_LE(bytes)) << BYTE_LE(offset)))
#define GET_NTH_BE(number, bytes, offset) (((size_t)number >> BYTE_BE(offset)) & MASK_LE(bytes))
#define GET_NTH_LE(number, bytes, offset) (((size_t)number >> BYTE_LE(offset)) & MASK_LE(bytes))

#define BYTE71(n7, n1) (SET_NTH_LE(n7, 7ULL, 0ULL) | SET_NTH_BE(n1, 1ULL, 0ULL))
#define BYTE71_GET_7(full) (GET_NTH_LE(full, 7ULL, 0))
#define BYTE71_GET_1(full) (GET_NTH_BE(full, 1ULL, 0))
#define BYTE71_SET_7(full, n7) (full | SET_NTH_LE(n7, 7ULL, 0))
#define BYTE71_SET_1(full, n1) (full | SET_NTH_BE(n1, 1ULL, 0))