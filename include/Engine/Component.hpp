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

#include <type_traits>
#include <memory>
#include "data/LinkedList.hpp"

extern "C"
{
#include "memory/alloc.h"
}

class Entity;
class Director;

enum ConfigEnum
{
	CAN_TICK = 1 << 0
};

class Entity;

class Director
{
private:
public:
	inline Director() {}

	template <class E>
	inline typename std::enable_if<std::is_base_of<Entity, E>::value>::type AddEntity()
	{
		auto i = new E(this);
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
	SinglyLinkedList<Entity> *m_entities;
	ArenaMemory *m_entityMemory;
};

inline Director *MakeDirector()
{
	Director *director = new Director();
	return director;
}

class Entity
{
private:
	Director *director;

public:
	ConfigEnum config = CAN_TICK;
	bool IsAlive = true;
	bool IsCreated = false;

	inline Entity(Director *director)
	{
		this->director = director;
	}

public:
	virtual void Create() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

public:
	inline void *
	operator new(size_t size)
	{
		printf("entity: %zu bytes\n", size);
		return arena_alloc(alloc->global, size, sizeof(size_t));
	}
};