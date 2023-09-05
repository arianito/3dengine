#pragma once

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