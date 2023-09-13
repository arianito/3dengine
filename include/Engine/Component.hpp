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

#include <stddef.h>
#include <type_traits>
#include <unordered_map>
#include "mem/traits.hpp"

extern "C"
{
#include "mem/alloc.h"
#include "mem/pool.h"
}

class Component;
class Entity;
class Director;

inline size_t nextComponentId()
{
	static size_t lastID{0u};
	return lastID++;
}

// classes

class Component
{
	size_t mEntityId;
	Component() : mEntityId(0) {}

	virtual void Create() = 0;
	virtual void Update() = 0;
};

class Entity
{
private:
	size_t mEntityId;
	std::unordered_map<size_t, Component *, PoolTrait<size_t>> mComponents;

public:
	Entity(size_t id) : mEntityId(id) {}

	size_t GetId()
	{
		return mEntityId;
	}
};

template <class T>
inline size_t GetComponentTypeId() noexcept
{
	static_assert(std::is_base_of<Component, T>::value, "T must be a base class of Component");
	static size_t id{nextComponentId()};
	return id;
}

// director

class Director
{
private:
	std::unordered_map<size_t, Entity *, std::hash<size_t>, std::equal_to<size_t>, PoolTrait<std::pair<const size_t, Entity *>>> mEntities;
	PoolMemory *mEntityMemory;
	size_t mEntityCouter;

public:
	inline Director(size_t capacity)
	{
		mEntityMemory = pool_create(alloc_global(void, capacity), capacity, sizeof(Entity));
		mEntityCouter = 0;
	}

	inline size_t CreateEntity()
	{
		void *ptr = pool_alloc(mEntityMemory);
		auto a = new (ptr) Entity(mEntityCouter++);
		mEntities.emplace(a->GetId(), a);
		printf("after emplace: %d \n", mEntityMemory->capacity);
		return a->GetId();
	}

	inline void Create()
	{
	}

	inline void Update()
	{
	}

	inline void *operator new(size_t size)
	{
		printf("director: %zu bytes\n", size);
		return arena_alloc(alloc->global, size, sizeof(size_t));
	}

private:
};

inline Director *MakeDirector()
{
	Director *director = new Director(2 * MEGABYTES);
	return director;
}