#pragma once

#include "input.h"
#include "memory/arena.h"
#include "memory/utils.h"

ArenaMemory *arena = NULL;

void memorydebug_create()
{
	arena = make_arena(120);
}

void memorydebug_update()
{

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)arena->size, 4}}, color_gray);
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)arena->offset, 3}}, color_yellow);

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)arena->offset, 20}}, color_red);

	if (input_keydown(KEY_SPACE))
	{
		size_t newSize = (size_t)(randf() * 20) + 3;
		void *ptr = arena_alloc(arena, newSize, 8);
		if (ptr != NULL)
		{
			printf("alloc %zu \n", arena->offset);
		}
	}
}