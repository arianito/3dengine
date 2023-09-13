#pragma once

extern "C"
{
#include "draw.h"
}

#include <memory>
#include "mem/freelist.h"
#include "game.h"
#include "mathf.h"
#include "data/Array.hpp"
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

class SomeObject : public Object<SomeObject>
{
public:
	int hello = 21;
	size_t value = 2;
};

struct GameWindow : public Object<GameWindow>
{
	Array<std::unique_ptr<SomeObject>> *array;
	float lastTime = -1;
	size_t capacity = 0;

	GameWindow()
	{
		array = new Array<std::unique_ptr<SomeObject>>(GlobalFreelistAllocator::instance());
	}

	~GameWindow()
	{
		delete array;
	}

	inline void Create()
	{
	}

	inline void Update()
	{
		debug_origin(vec2_zero);
		debug_string3df(transform_identity, "freelist: %zu / %zu \n len: %d / %d", capacity, alloc->freelist->size, array->size(), array->capacity());

		if (input_keydown(KEY_SPACE))
		{
			SomeObject *obj2 = new SomeObject();
			obj2->hello = array->size();
			std::unique_ptr<SomeObject> ptr(obj2);
			array->prepend(ptr);
		}

		if (input_keydown(KEY_M))
		{
			if (array->size() > 0)
				array->remove(array->size() - 1);
		}
		if (input_keydown(KEY_N))
		{
			delete array;
			array = new Array<std::unique_ptr<SomeObject>>(GlobalFreelistAllocator::instance());
		}

		FreeListMemory *freelist = alloc->freelist;

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

			fill_bbox(BBox{{-10, data, 0}, {10, next, 15}}, color_green);
			draw_bbox(BBox{{-10, data, 0}, {10, next, 15}}, color_black);

			node = (FreeListMemory *)node->next;
		}

		if (gameTime->time - lastTime > 1)
		{
			capacity = freelist_capacity(alloc->freelist);
			lastTime = gameTime->time;
		}
	}
};