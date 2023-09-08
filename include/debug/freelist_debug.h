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

#include "../input.h"
#include "../mathf.h"
#include "../draw.h"
#include "../sort.h"
#include "../game.h"
#include "../memory/freelist.h"
#include "../memory/utils.h"
#include "../memory/memory.h"
#include "../camera.h"

FreeListMemory *freelist = NULL;
enum
{
	npool = 200,
};
size_t pools[npool];

float lastHit = 0;
float flyTime = -90;

void memorydebug_create()
{
	freelist = make_freelist(1024);
	clear(pools, sizeof(pools));
}

void memorydebug_update()
{
	fill_bbox(BBox{{-15, 0, -10}, {15, (float)freelist->size, -1}}, color_gray);

	const unsigned int space = MEMORY_SPACE_STD(FreeListMemory);
	size_t cursor = freelist->padding;

	size_t start = (size_t)freelist + space - freelist->padding;
	FreeListMemory *node = (FreeListMemory *)freelist->next;

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)(cursor), 40}}, color_darkred);
	while (node != NULL)
	{
		size_t address = (size_t)node - start;

		float block = (float)(address + space - node->padding);
		float head = (float)(address);
		float data = (float)(address + space);
		float next = (float)(block + node->size);

		draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		fill_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_alpha(color_green, 0.25));
		draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_black);

		node = (FreeListMemory *)node->next;
	}

	for (int i = 0; i < npool; i++)
	{
		if (pools[i] != 0)
		{
			FreeListMemory *node = (FreeListMemory *)(pools[i] - space);

			size_t address = (size_t)node - start;

			float block = (float)(address + space - node->padding);
			float head = (float)(address);
			float data = (float)(address + space);
			float next = (float)(block + node->size);

			draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
			draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
			fill_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_alpha(color_red, 0.25));
			draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_black);
		}
	}

	if (input_keypress(KEY_SPACE) && (time->time - lastHit > 0.1f))
	{
		sort_quick(pools, 0, npool - 1);
		void *ptr = (void *)pools[0];
		if (ptr == NULL)
		{

			size_t newSize = (size_t)(randf() * 120 + 1);
			void *newPtr = freelist_alloc(freelist, newSize, 8);
			if (newPtr != NULL)
			{
				pools[0] = (size_t)newPtr;
			}
		}
		lastHit = time->time;
	}
	if (input_keypress(KEY_M) && (time->time - lastHit > 0.1f))
	{
		sort_quick(pools, 0, npool - 1);
		int a = npool - 1;
		for (int i = npool - 1; i >= 0; i--)
		{
			if (pools[i] != 0 && i < a)
			{
				a = i;
			}
		}
		sort_shuffle(pools, a, npool - 1);

		void *ptr = (void *)pools[npool - 1];
		if (ptr != NULL)
			if (freelist_free(freelist, &ptr))
				pools[npool - 1] = 0;

		lastHit = time->time;
	}
	if (input_keydown(KEY_N))
	{
		freelist_reset(freelist);
		clear(pools, sizeof(pools));
	}
}