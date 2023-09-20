#pragma once


#include <type_traits>
#include <new>
#include <cassert>

extern "C" {
#include "mem/alloc.h"
#include "mem/slab.h"
}

#include "engine/Memory.hpp"
#include "data/Array.hpp"
#include "data/ProbeHashTable.hpp"


// alloc
inline void *ecs_global_allocator(size_t size) {
    return arena_alloc(alloc->global, size, sizeof(size_t));
}

// types
class Entity;

class Component;

class BaseSystem;

class Director;

typedef int ComponentId;
typedef int SystemId;
typedef int EntityId;

using EntityMap = ProbeHashTable<EntityId, Entity *>;
using ComponentMap = ProbeHashTable<ComponentId, Component *>;
using SystemMap = ProbeHashTable<SystemId, BaseSystem *>;
using EntityComponentMap = ProbeHashTable<EntityId, Component *>;
using ComponentEntityComponentMap = ProbeHashTable<ComponentId, EntityComponentMap *>;

// utils

static inline ComponentId nextComponentId() {
    static ComponentId lastID{0};
    return lastID++;
}

static inline SystemId nextSystemId() {
    static SystemId lastID{0};
    return lastID++;
}


template<class T>
static inline ComponentId GetComponentTypeId() noexcept {
    static_assert(std::is_base_of_v<Component, T>, "T must be a base class of Component");
    static ComponentId id{nextComponentId()};
    return id;
}

template<class T>
static inline SystemId GetSystemTypeId() noexcept {
    static_assert(std::is_base_of_v<BaseSystem, T>, "T must be a base class of System");
    static SystemId id{nextComponentId()};
    return id;
}

// entity

class Entity {
private:
    ComponentMap mComponents;
    EntityId mEntityId;

public:
    explicit inline Entity(EntityId id) : mEntityId(id) {}

    explicit inline Entity(const Entity &) = delete;


    template<class T>
    inline void AddComponent(T *component) {
        ComponentId id = GetComponentTypeId<T>();
        assert(!mComponents.Contains(id) && "Entity: already has component");
        mComponents.Set(id, component);

    }

    [[nodiscard]] inline ComponentMap &GetComponents() { return mComponents; }

    [[nodiscard]] inline const EntityId &GetEntityId() const { return mEntityId; }
};

// component

class Component {
private:
    EntityId mEntityId;

public:
    explicit inline Component() : mEntityId(0) {}

    [[nodiscard]] inline const EntityId &GetEntityId() const {
        return mEntityId;
    }

    inline void SetEntityId(EntityId id) {
        mEntityId = id;
    }
};



// system

class BaseSystem {
public:
    virtual void Create() {}

    virtual void Update() {}

protected:
    virtual void OnEntityCreated(Entity *entity) = 0;

    virtual void OnEntityDestroyed(EntityId entity) = 0;

    bool mShouldUpdate = false;

    friend class Director;
};

template<class ...Components>
class System : public BaseSystem {
protected:
    friend class Director;

    using CTuple = std::tuple<std::add_pointer_t<Components>...>;
    Director *mDirector{nullptr};
    Array<CTuple> mComponents;
    ProbeHashTable<EntityId, int> mEntityIndex;

    template<int Index, class Type, class... Args>
    inline bool hasComponent(ComponentId id, Component *component, CTuple &tuple) {
        if (GetComponentTypeId<Type>() == id) {
            std::get<Index>(tuple) = static_cast<Type *>(component);
            return true;
        }

        return hasComponent<Index + 1, Args...>(id, component, tuple);
    }

    template<int Index>
    inline bool hasComponent(ComponentId id, Component *component, CTuple &tuple) {
        return false;
    }

    inline void OnEntityCreated(Entity *entity) override {
        CTuple tuple;
        int matches = 0;
        for (auto component: entity->GetComponents()) {
            if (hasComponent<0, Components...>(component.first, component.second, tuple)) {
                ++matches;
                if (matches == sizeof...(Components)) {
                    mComponents.Add(std::move(tuple));
                    mEntityIndex.Set(entity->GetEntityId(), mComponents.Length() - 1);
                    mShouldUpdate = mComponents.Length() > 0;
                    break;
                }
            }
        }
    }

    inline void OnEntityDestroyed(EntityId entityId) override {
        if (!mEntityIndex.Contains(entityId))
            return;

        auto index = mEntityIndex[entityId];
        mComponents[index] = std::move(mComponents[mComponents.Length() - 1]);
        mComponents.Pop();
        const auto movedEntity = std::get<0>(mComponents[index]);
        auto movedEntityId = movedEntity->GetEntityId();
        if (!mEntityIndex.Contains(movedEntityId))
            return;
        mEntityIndex[movedEntityId] = index;
        mShouldUpdate = mComponents.Length() > 0;

    }

    inline void SetDirector(Director *director) {
        mDirector = director;
    }

public:

    inline Array<CTuple> &GetComponents() {
        return mComponents;
    }

    [[nodiscard]] inline Director *GetDirector() const {
        return mDirector;
    }

};

// director

class Director {
private:
    EntityId mEntityCounter{0};

    EntityMap mEntities;
    ComponentEntityComponentMap mComponents;
    SystemMap mSystems;

    SlabMemory *mEntitySlab;
    ProbeHashTable<ComponentId, SlabMemory *> mComponentsSlab;
    ProbeHashTable<SystemId, SlabMemory *> mSystemsSlab;

    template<class T>
    inline SlabMemory *makeSlab(int length) {
        void *m = arena_alloc(alloc->global, sizeof(SlabMemory), sizeof(size_t));
        SlabMemory *slab = slab_create(m, sizeof(T) * length, sizeof(T));
        slab->allocator = ecs_global_allocator;
        return slab;
    }

    template<class T>
    inline SlabMemory *getComponentSlab() {
        ComponentId id = GetComponentTypeId<T>();
        if (mComponentsSlab.Contains(id))
            return mComponentsSlab[id];
        SlabMemory *slab = makeSlab<T>(20);
        mComponentsSlab.Set(id, slab);
        return slab;
    }

    template<class T>
    inline SlabMemory *getSystemSlab() {
        SystemId id = GetSystemTypeId<T>();
        if (mSystemsSlab.Contains(id))
            return mSystemsSlab[id];
        SlabMemory *slab = makeSlab<T>(20);
        mSystemsSlab.Set(id, slab);
        return slab;
    }

    inline SlabMemory *getComponentSlab(ComponentId id) {
        if (mComponentsSlab.Contains(id))
            return mComponentsSlab[id];
        return nullptr;
    }

public:
    explicit inline Director() {
        mEntitySlab = makeSlab<Entity>(20);
    }

    explicit inline Director(const Director &) = delete;

    inline EntityId CreateEntity() {
        auto entity = new(slab_alloc(mEntitySlab)) Entity{mEntityCounter++};
        mEntities.Set(entity->GetEntityId(), entity);
        return entity->GetEntityId();
    }

    inline void DestroyEntity(EntityId entityId) {
        assert(mEntities.Contains(entityId) && "ECS: entity not found");
        Entity *entity = mEntities[entityId];
        mEntities.Remove(entityId);
        slab_free(mEntitySlab, (void **) &entity);
        for (auto p: mComponents) {
            ComponentId componentId = p.first;
            auto &entityComponents = p.second;
            if (entityComponents->Contains(entityId)) {
                auto component = (*entityComponents)[entityId];
                component->~Component();
                auto slab = getComponentSlab(componentId);
                if (slab) slab_free(slab, (void **) &component);
            }
        }
        for (auto system: mSystems)
            system.second->OnEntityDestroyed(entityId);
    }

    template<class T, class... Args>
    inline T *AddComponent(EntityId entityId, Args &&...args) {
        assert(mEntities.Contains(entityId) && "ECS: entity not found");

        EntityComponentMap *entityComponents;

        auto entity = mEntities[entityId];

        ComponentId id = GetComponentTypeId<T>();
        if (mComponents.Contains(id)) {
            entityComponents = mComponents[id];
            if (entityComponents->Contains(entityId)) {
                return (T *) ((*entityComponents)[entityId]);
            }
        } else {
            void *ptr = freelist_alloc(alloc->freelist, sizeof(EntityComponentMap), sizeof(size_t));
            entityComponents = new(ptr) EntityComponentMap();
            mComponents.Set(id, entityComponents);
        }

        T *component = new(slab_alloc(getComponentSlab<T>())) T(std::forward<Args>(args)...);
        component->SetEntityId(entityId);

        entity->AddComponent(component);
        entityComponents->Set(entityId, component);

        return component;
    }

    inline void Commit(EntityId entityId) {
        assert(mEntities.Contains(entityId) && "ECS: entity not found");
        auto entity = mEntities[entityId];
        for (auto sys: mSystems) {
            sys.second->OnEntityCreated(entity);
        }
    }

    template<class T, class... Args>
    inline T *AddSystem(Args &&...args) {
        SystemId id = GetSystemTypeId<T>();
        if (mSystems.Contains(id))
            return (T *) (mSystems[id]);
        T *sys = new(slab_alloc(getSystemSlab<T>())) T(std::forward<Args>(args)...);
        sys->SetDirector(this);
        mSystems.Set(id, sys);
        return sys;
    }

    inline void Create() {
        for (auto sys: mSystems) {
            sys.second->Create();
        }
    }

    inline void Update() {
        for (auto sys: mSystems) {
            if (sys.second->mShouldUpdate) {
                sys.second->Update();
            }
        }
    }

};
