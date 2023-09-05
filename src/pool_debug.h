#pragma once

#include "input.h"
#include "memory/pool.h"
#include "memory/utils.h"
#include "memory/memory.h"
#include "sort.h"

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
	int space = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	size_t cursor = pool->padding + space;
	space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)pool->size, 4}}, color_gray);
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)(cursor), 40}}, color_darkred);


	while (1)
	{
		size_t chunk_address = start + cursor;
		size_t padding = calculate_alignment(chunk_address, sizeof(PoolMemoryNode), sizeof(size_t));


		if (cursor + padding + chunk_size > pool->size)
			break;

		cursor += padding + chunk_size;

		PoolMemoryNode *node = (PoolMemoryNode *)((chunk_address + padding) - space);

		unsigned char used = 0;
		byte7d(node->data, NULL, &used);

		float head = (float)((size_t)node - start);
		float data = (float)((size_t)node - start + space);
		float next = (float)((size_t)node - start + space + chunk_size);
		Color c = used ? color_red : color_green;
		if (pool->head == node)
			c = color_yellow;
		draw_bbox(BBox{{-10, data, 0}, {10, next, 25.0f}}, c);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
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