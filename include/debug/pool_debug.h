#pragma once

#include "../input.h"
#include "../mathf.h"
#include "../draw.h"
#include "../sort.h"
#include "../memory/pool.h"
#include "../memory/utils.h"
#include "../memory/memory.h"

PoolMemory *pool = NULL;
enum
{
	npool = 20,
};
size_t pools[npool];
int chunk_size = 23;
int capacity = 0;

void memorydebug_create()
{
	pool = make_pool(300, chunk_size);
	printf("capacity:::: %d \n", pool->capacity);
	capacity = pool->capacity;
	clear(pools, sizeof(pools));
}

void memorydebug_update()
{

	size_t start = (size_t)pool - pool->padding;
	unsigned int space = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	size_t cursor = pool->padding + space;
	space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)pool->size, 4}}, color_gray);
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)(cursor), 40}}, color_darkred);

	while (1)
	{
		size_t chunkAddress = start + cursor;
		size_t padding = calculate_alignment(chunkAddress, sizeof(PoolMemoryNode), sizeof(size_t));

		if (cursor + padding + chunk_size > pool->size)
			break;

		cursor += padding + chunk_size;
		PoolMemoryNode *node = (PoolMemoryNode *)(chunkAddress + padding - space);

		size_t address = (size_t)node - start;

		float block = (float)(address + space - padding);
		float head = (float)(address);
		float data = (float)(address + space);
		float next = (float)(data + chunk_size);

		unsigned char used = 0;
		byte7d(node->data, NULL, &used);
		Color c = used ? color_red : color_green;
		if (pool->head == node)
			c = color_yellow;

		draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		draw_bbox(BBox{{-10, data, 0}, {10, next, 25}}, c);
	}

	draw_bbox(BBox{{-10, (float)(cursor), 0}, {10, (float)(pool->size), 40}}, color_darkred);

	if (input_keydown(KEY_SPACE))
	{
		sort_quick(pools, 0, npool - 1);
		void *ptr = (void *)pools[0];
		if (ptr == NULL)
		{

			void *newPtr = pool_alloc(pool);
			if (newPtr != NULL)
			{
				pools[0] = (size_t)newPtr;
				printf("alloc %d \n", pool->capacity);
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
		{
			if (pool_free(pool, &ptr))
			{
				printf("free %d \n", pool->capacity);
				pools[npool - 1] = 0;
			}
		}
	}
	if (input_keydown(KEY_N))
	{
		if (capacity == pool->capacity)
		{
			size_t newSize = (size_t)(randf() * 1000);
			pool_destroy(&pool);
			pool = make_pool(newSize, chunk_size);
			capacity = pool->capacity;
		}
	}
}