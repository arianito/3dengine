#pragma once


#include <cstddef>
#include <type_traits>
#include <unordered_map>

extern "C"
{
#include "mem/alloc.h"
#include "mem/pool.h"
}

class Component;

class Entity;

class Director;

static inline size_t nextComponentId() {
    static size_t lastID{0u};
    return lastID++;
}

// classes

class Component {
    size_t mEntityId;

    Component() : mEntityId(0) {}

    virtual void Create() = 0;

    virtual void Update() = 0;
};

class Entity {
private:
    size_t mEntityId;
    std::unordered_map<size_t, Component *> mComponents;

public:
    explicit Entity(size_t id) : mEntityId(id) {}

    size_t GetId() const {
        return mEntityId;
    }
};

template<class T>
static inline size_t GetComponentTypeId() noexcept {
    static_assert(std::is_base_of<Component, T>::value, "T must be a base class of Component");
    static size_t id{nextComponentId()};
    return id;
}

// director

class Director {
private:
    std::unordered_map<size_t, Entity *> mEntities;
    PoolMemory *mEntityMemory;
    size_t mEntityCouter;

public:
    explicit inline Director(size_t capacity) {
        mEntityMemory = pool_create(alloc_global(void, capacity), capacity, sizeof(Entity));
        mEntityCouter = 0;
    }

    explicit inline Director(const Director &) = delete;

    inline size_t CreateEntity() {
        void *ptr = pool_alloc(mEntityMemory);
        auto a = new(ptr) Entity(mEntityCouter++);
        mEntities.emplace(a->GetId(), a);
        printf("after emplace: %d \n", mEntityMemory->capacity);
        return a->GetId();
    }

    inline void Create() {
    }

    inline void Update() {
    }

    inline void *operator new(size_t size) {
        printf("director: %zu bytes\n", size);
        return arena_alloc(alloc->global, size, sizeof(size_t));
    }

    inline void operator delete(void *ptr) {
        // Do nothing
    }

private:
};

static inline Director *MakeDirector() {
    auto director = new Director(2 * MEGABYTES);
    return director;
}