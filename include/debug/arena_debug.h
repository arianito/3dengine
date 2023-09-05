#pragma once

#include "../input.h"
#include "../mathf.h"
#include "../draw.h"
#include "../memory/arena.h"
#include "../memory/utils.h"

ArenaMemory *arena = NULL;

void memorydebug_create()
{
	arena = make_arena(120);
}

void memorydebug_update()
{

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)arena->size, 5}}, color_gray);

	int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
	float end = (float)(space + arena->padding);

	draw_bbox(BBox{{-10, 0, 0}, {10, end, 40}}, color_darkred);
	draw_bbox(BBox{{-10, end, 0}, {10, (float)arena->offset, 6}}, color_yellow);

	draw_bbox(BBox{{-10, end, 0}, {10, (float)arena->offset, 25.0f}}, color_red);

	if (space + arena->padding != arena->offset)
	{
		draw_bbox(BBox{{-10, (float)arena->offset, 0}, {10, (float)arena->size, 40}}, color_darkred);
	}

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