#pragma once

#include "input.h"
#include "memory/stack.h"
#include "memory/utils.h"

StackMemory *stack = NULL;
int j = 0;
void *pools[10];

void memorydebug_create()
{
	stack = make_stack(512);
}

void memorydebug_update()
{

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)stack->size, 4}}, color_gray);
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)stack->offset, 3}}, color_yellow);
	draw_bbox(BBox{{-10, 0, 0}, {10, (float)stack->peak, 2}}, color_blue);

	size_t start = (size_t)stack - stack->padding;
	int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));

	StackMemoryNode *node = (StackMemoryNode *)stack->head;
	
	float end = (float)stack->offset;
	while (node != NULL)
	{

		float head = (float)((size_t)node - start);
		float data = (float)((size_t)node - start + space);


		draw_bbox(BBox{{-10, head, 8}, {10, data, 10}}, color_red);
		draw_bbox(BBox{{-10, data, 0}, {10, end, 20}}, color_green);


		node = (StackMemoryNode *)node->next;
		end = head;
	}

	if (input_keydown(KEY_SPACE))
	{
		if (j < 10)
		{
			size_t newSize = (size_t)(randf() * 50) + 3;
			void *ptr = stack_alloc(stack, newSize, 8);
			if (ptr != NULL)
			{
				printf("alloc %d -> %zu \n", j, stack->offset);
				pools[j] = ptr;
				j++;
			}
		}
	}

	if (input_keydown(KEY_M))
	{
		if (j > 0)
		{
			j--;
			if (stack_free(stack, &pools[j]))
			{
				printf("free %d -> %d \n", j, stack->offset);
			}
		}
	}
}