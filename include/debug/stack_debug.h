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
#include "../game.h"
#include "../camera.h"
#include "../memory/stack.h"
#include "../memory/utils.h"

StackMemory *stack = NULL;
int j = 0;
void *pools[200];
float flyTime = -90;
float lastHit = 0;

void memorydebug_create()
{
	stack = make_stack(1024);

	size_t a = 0x657;
	unsigned char b = 0x87;
}

void memorydebug_update()
{
	fill_bbox(BBox{{-15, 0, -10}, {15, (float)stack->size, -1}}, color_gray);

	int space = MEMORY_SPACE_STD(StackMemory);
	float end = (float)(space + stack->padding);

	draw_bbox(BBox{{-10, 0, 0}, {10, end, 40}}, color_darkred);
	draw_bbox(BBox{{-10, end, 0}, {10, (float)stack->offset, 6}}, color_yellow);
	draw_bbox(BBox{{-10, end, 0}, {10, (float)stack->peak, 4}}, color_blue);

	if (stack->peak != stack->offset)
	{
		draw_bbox(BBox{{-10, (float)stack->peak, 0}, {10, (float)stack->size, 40}}, color_darkred);
	}

	size_t start = (size_t)stack - stack->padding;
	space = MEMORY_SPACE_STD(StackMemoryNode);
	StackMemoryNode *node = (StackMemoryNode *)stack->head;

	end = (float)stack->offset;
	while (node != NULL)
	{

		size_t offset = BYTE71_GET_7(node->data);
		unsigned char pad = BYTE71_GET_1(node->data);

		size_t address = (size_t)node - start;

		float block = (float)(address + space - pad);
		float head = (float)(address);
		float data = (float)(address + space);

		Color c = color_red;
		if (stack->head == node)
			c = color_yellow;

		draw_bbox(BBox{{-10, block, 15}, {10, head, 20}}, color_white);
		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		fill_bbox(BBox{{-10, data, 0}, {10, end, 25}}, c);
		draw_bbox(BBox{{-10, data, 0}, {10, end, 25}}, color_black);

		if (offset == 0)
			break;
		node = (StackMemoryNode *)(start + offset);
		end = block;
	}

	if (input_keypress(KEY_SPACE) && (time->time - lastHit > 0.1f))
	{
		if (j < 200)
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
		lastHit = time->time;
	}
	if (input_keypress(KEY_M) && (time->time - lastHit > 0.1f))
	{
		if (j > 0)
		{
			j--;
			if (stack_free(stack, &pools[j]))
			{
				printf("free %d -> %d \n", j, stack->offset);
			}
		}
		lastHit = time->time;
	}

	if (input_keypress(KEY_ENTER))
	{
		// Vec3 forward = vec3(-300, sind(flyTime) * 512 + 512, 120);
		// camera->rotation = rot(-15, 0, 0);
		// camera->position = forward;
		// camera_update();
		// flyTime += time->deltaTime * 10.0f;

		Vec3 forward = vec3(0, sind(flyTime) * 512 + 400, 15);
		camera->rotation = rot(0, 90, 0);
		camera->position = forward;
		camera_update();
		flyTime += time->deltaTime * 10.0f;
	}
}