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
	npool = 20,
};
size_t pools[npool];

void memorydebug_create()
{
	freelist = make_freelist(256);
	clear(pools, sizeof(pools));
	size_t out = 0x1245000000000000;
	byte6a6(&out, 0x653787);
	byte6a6(&out, 0x23);
	printf("--- 0x%zx \n", out);
}

void memorydebug_update()
{
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)freelist->size, 5}}, color_gray);

	unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	size_t cursor = freelist->padding + space;

	size_t start = (size_t)freelist - freelist->padding;
	space = calculate_space(sizeof(FreeListMemoryNode), sizeof(size_t));
	FreeListMemoryNode *node = (FreeListMemoryNode *)freelist->head;

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)(cursor), 40}}, color_darkred);
	int i = 0;
	while (node != NULL && i < 1000)
	{
		size_t size;
		unsigned short pad;
		byte6d(node->data, &size, &pad);
		size_t address = (size_t)node - start;

		float block = (float)(address + space - pad);
		float head = (float)(address);
		float data = (float)(address + space);
		float next = (float)(block + size);

		draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_green);

		node = (FreeListMemoryNode *)node->next;
		i ++;
	}

	for (int i = 0; i < npool; i++)
	{
		if (pools[i] != 0)
		{
			FreeListMemoryNode *node = (FreeListMemoryNode *)(pools[i] - space);

			size_t size;
			unsigned short pad;
			byte6d(node->data, &size, &pad);
			size_t address = (size_t)node - start;

			float block = (float)(address + space - pad);
			float head = (float)(address);
			float data = (float)(address + space);
			float next = (float)(block + size);

			draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
			draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
			draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, color_red);
		}
	}

	if (input_keydown(KEY_SPACE))
	{
		sort_quick(pools, 0, npool - 1);
		void *ptr = (void *)pools[0];
		if (ptr == NULL)
		{

			size_t newSize = (size_t)(randf() * 48 + 5);
			void *newPtr = freelist_alloc(freelist, newSize, 8);
			if (newPtr != NULL)
			{
				pools[0] = (size_t)newPtr;
			}
		}
	}
	if (input_keydown(KEY_M))
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