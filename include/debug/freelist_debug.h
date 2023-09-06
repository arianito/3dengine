#pragma once

#include "../input.h"
#include "../mathf.h"
#include "../draw.h"
#include "../sort.h"
#include "../memory/freelist.h"
#include "../memory/utils.h"
#include "../memory/memory.h"

FreeListMemory *freelist = NULL;
enum
{
	npool = 200,
};
size_t pools[npool];

void memorydebug_create()
{
	freelist = make_freelist(2048);
	clear(pools, sizeof(pools));

	printf("0x%zx\n", BYTE_MASK(size_t, short));
	printf("0x%zx\n", ~BYTE_MASK(size_t, short));
}

void memorydebug_update()
{
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)freelist->size, 5}}, color_gray);

	unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	size_t cursor = freelist->padding;

	size_t start = (size_t)freelist + space - freelist->padding;
	FreeListMemory *node = (FreeListMemory *)freelist->next;

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)(cursor), 40}}, color_darkred);
	int i = 0;
	while (node != NULL && i < 1000)
	{
		size_t address = (size_t)node - start;

		float block = (float)(address + space - node->padding);
		float head = (float)(address);
		float data = (float)(address + space);
		float next = (float)(block + node->size);

		draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_green);

		node = (FreeListMemory *)node->next;
		i++;
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
			draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_red);
		}
	}

	if (input_keypress(KEY_SPACE))
	{
		sort_quick(pools, 0, npool - 1);
		void *ptr = (void *)pools[0];
		if (ptr == NULL)
		{

			size_t newSize = (size_t)(randf() * 76 + 1);
			void *newPtr = freelist_alloc(freelist, newSize, (int)((powf(2, (int)randf() * 5 + 3)) * 8));
			if (newPtr != NULL)
			{
				pools[0] = (size_t)newPtr;
			}
		}
	}
	if (input_keypress(KEY_M))
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
	}
	if (input_keydown(KEY_N))
	{
		freelist_reset(freelist);
		clear(pools, sizeof(pools));
	}
}