
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
#include "file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory/alloc.h"

void resolve(const char *p, char out[URL_LENGTH])
{
	sprintf_s(out, URL_LENGTH, "%s%s", file->prefix, p);
}

void file_init(const char *pfx)
{
	file = alloc_global(FileData);
	clear(file, sizeof(FileData));
	int n = (int)strlen(pfx);
	if (n > 128)
		n = 128;
	memcpy(file->prefix, pfx, n);
}

File *file_read(const char *p, char endline)
{
	char b[URL_LENGTH];
	resolve(p, b);

	FILE *f;
	fopen_s(&f, b, "r");

	if (f == NULL)
	{
		perror("failed to open the file");
		exit(EXIT_FAILURE);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	File *buff = alloc_stack_size(File, sizeof(File) + file_size + 1);
	buff->length = file_size;
	buff->text = (char *)((size_t)buff + sizeof(File));
	size_t bytes_read = fread(buff->text, 1, file_size, f);
	if (endline)
		buff->text[bytes_read] = '\0';
	fclose(f);
	return buff;
}

void file_destroy(File *f)
{
	alloc_free(f);
}