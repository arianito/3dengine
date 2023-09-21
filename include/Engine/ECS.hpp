#pragma once


extern "C" {
#include "mem/alloc.h"
#include "mem/slab.h"
#include "debug.h"
}

#include <type_traits>
#include <typeinfo>
#include <new>
#include <cassert>

#include "engine/Memory.hpp"
#include "data/Array.hpp"
#include "data/ProbeHashTable.hpp"
#include "data/FixedStack.hpp"

class Entity;

class Component;

class BaseSystem;

class Director;

typedef int BaseType;
typedef BaseType ComponentId;
typedef BaseType SystemId;
typedef BaseType EntityId;
using EntityMap = ProbeHashTable<EntityId, Entity *>;
using ComponentMap = ProbeHashTable<ComponentId, Component *>;
using SystemMap = ProbeHashTable<SystemId, BaseSystem *>;
using EntityComponentMap = ProbeHashTable<EntityId, Component *>;
using ComponentEntityComponentMap = ProbeHashTable<ComponentId, EntityComponentMap *>;
using EntityIndexMap = ProbeHashTable<EntityId, int>;
using EntityIdStack = FixedStack<EntityId>;
using EntityPtrStack = FixedStack<Entity *>;
using PartialSlabMemory = ProbeHashTable<BaseType, SlabMemory *>;

static inline ComponentId nextComponentId() {
    static ComponentId lastID{1};
    return lastID++;
}

static inline SystemId nextSystemId() {
    static SystemId lastID{1};
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
    static SystemId id{nextSystemId()};
    return id;
}

class Entity {
private:
    ComponentMap mComponents;
    EntityId mEntityId;
public:
    explicit inline Entity(EntityId id) : mEntityId(id) {}

    explicit inline Entity(const Entity &) = delete;

    virtual ~Entity() = default;

    template<class T>
    inline void AddComponent(T *component) {
        ComponentId id = GetComponentTypeId<T>();
        assert(!mComponents.Contains(id) && "Entity: already has component");
        mComponents.Set(id, component);
    }

    template<class T>
    inline T *GetComponent() {
        ComponentId id = GetComponentTypeId<T>();
        assert(!mComponents.Contains(id) && "Entity: already has component");
        return mComponents[id];
    }

    template<class T>
    inline bool HasComponent() {
        ComponentId id = GetComponentTypeId<T>();
        return mComponents.Contains(id);
    }

    [[nodiscard]] inline ComponentMap &GetComponents() { return mComponents; }

    [[nodiscard]] inline EntityId GetEntityId() const { return mEntityId; }
};

class Component {
private:
    EntityId mEntityId;
public:
    explicit inline Component() : mEntityId(0) {}

    virtual ~Component() = default;

    [[nodiscard]] inline EntityId GetEntityId() const { return mEntityId; }

    inline void SetEntityId(EntityId id) { mEntityId = id; }
};

class BaseSystem {
public:
    virtual void Create() {}

    virtual void Update() {}

protected:
    friend class Director;

    Director *mDirector{nullptr};

    virtual ~BaseSystem() = default;

    virtual void Process() = 0;

    virtual void OnEntityCreated(Entity *entity) = 0;

    virtual void OnEntityDestroyed(EntityId entity) = 0;

    bool mShouldUpdate = false;
};

template<class ...Components>
class System : public BaseSystem {
protected:
    using CTuple = std::tuple<std::add_pointer_t<Components>...>;
    Array<CTuple> mComponents;
    EntityIndexMap mEntityIndex;
    EntityIdStack mDestroyStack;
    EntityPtrStack mCreateStack;

    template<int Index, class Type, class... Args>
    inline bool hasComponent(ComponentId id, Component *component, CTuple &tuple) {
        if (GetComponentTypeId<Type>() == id) {
            std::get<Index>(tuple) = static_cast<Type *>(component);
            return true;
        }
        return hasComponent<Index + 1, Args...>(id, component, tuple);
    }

    template<int Index>
    inline bool hasComponent([[maybe_unused]] ComponentId id, [[maybe_unused]]  Component *component, [[maybe_unused]]  CTuple &tuple) {
        return false;
    }

    inline void OnEntityCreated(Entity *entity) override {
        mCreateStack.Push(entity);
    }

    inline void OnEntityDestroyed(EntityId entityId) override {
        if (!mEntityIndex.Contains(entityId)) return;
        mDestroyStack.Push(entityId);
    }

    inline void SetDirector(Director *director) { mDirector = director; }

    inline void Process() override;

public:
    inline Array<CTuple> &GetComponents() { return mComponents; }

    [[nodiscard]] inline Director *GetDirector() const { return mDirector; }

    friend class Director;
};

class Director {
private:
    EntityId mEntityCounter{1};
    EntityMap *mEntities;
    ComponentEntityComponentMap mComponents;
    SystemMap *mSystems;
    SlabMemory *mEntitySlab;
    PartialSlabMemory mComponentsSlab;
    PartialSlabMemory mSystemsSlab;
    EntityIdStack mDestroyStack;

    inline static void *ecs_global_alloc(size_t size) {
        return freelist_alloc(alloc->freelist, size, sizeof(size_t));
    }

    inline static void ecs_global_free(void *ptr) {
        freelist_free(alloc->freelist, &ptr);
    }

    template<class T>
    inline SlabMemory *makeSlab(int length) {
        SlabAllocator pAlloc;
        pAlloc.alloc = ecs_global_alloc;
        pAlloc.free = ecs_global_free;
        return slab_create_alloc(pAlloc, sizeof(T) * length, sizeof(T));
    }

    template<class T>
    inline SlabMemory *getComponentSlab() {
        ComponentId id = GetComponentTypeId<T>();
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        SlabMemory *slab = makeSlab<T>(20);
        mComponentsSlab.Set(id, slab);
        return slab;
    }

    template<class T>
    inline SlabMemory *getSystemSlab() {
        SystemId id = GetSystemTypeId<T>();
        if (mSystemsSlab.Contains(id)) return mSystemsSlab[id];
        SlabMemory *slab = makeSlab<T>(20);
        mSystemsSlab.Set(id, slab);
        return slab;
    }

    inline SlabMemory *getComponentSlab(ComponentId id) {
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        return nullptr;
    }

    inline void performDelete(EntityId entityId) {
        if (!mEntities->Contains(entityId))
            return;
        Entity *entity = (*mEntities)[entityId];

        for (auto p: mComponents) {
            ComponentId componentId = p.first;
            auto entityComponents = p.second;
            if (entityComponents->Contains(entityId)) {
                auto component = (*entityComponents)[entityId];
                component->~Component();
                auto slab = getComponentSlab(componentId);
                slab_free(slab, (void **) &component);
                entityComponents->Remove(entityId);
            }
        }
        entity->~Entity();
        slab_free(mEntitySlab, (void **) &entity);
        mEntities->Remove(entityId);
    }

public:
    explicit inline Director() {
        mEntitySlab = makeSlab<Entity>(20);
        mEntities = AllocNew<FreeListMemory, EntityMap>();
        mSystems = AllocNew<FreeListMemory, SystemMap>();
    }

    inline ~Director() {
        for (auto system: *mSystems) (system.second)->~BaseSystem();

        for (auto p: mComponentsSlab) slab_destroy(&(p.second));

        for (auto p: mSystemsSlab) slab_destroy(&(p.second));

        for (auto p: mComponents) {
            for (auto c: *(p.second)) {
                auto component = c.second;
                component->~Component();
            }
            auto map = p.second;
            map->~EntityComponentMap();
            Free<FreeListMemory>((void **) &map);
        }


        for (auto entity: *mEntities) (entity.second)->~Entity();
        mEntities->~EntityMap();
        Free<FreeListMemory>((void **) &mEntities);
        slab_destroy(&mEntitySlab);


        mSystems->~SystemMap();
        Free<FreeListMemory>((void **) &mSystems);
    }

    explicit inline Director(const Director &) = delete;

    inline EntityId CreateEntity() {
        auto entity = new(slab_alloc(mEntitySlab)) Entity(mEntityCounter++);
        mEntities->Set(entity->GetEntityId(), entity);
        return entity->GetEntityId();
    }

    inline void DestroyEntity(EntityId entityId) {
        if (!mEntities->Contains(entityId))
            return;

        for (auto system: (*mSystems))
            system.second->OnEntityDestroyed(entityId);

        mDestroyStack.Push(entityId);
    }

    template<class T, class... Args>
    inline T *AddComponent(EntityId entityId, Args &&...args) {
        if (!mEntities->Contains(entityId))
            return nullptr;
        EntityComponentMap *entityComponents;
        auto entity = (*mEntities)[entityId];
        ComponentId id = GetComponentTypeId<T>();
        if (mComponents.Contains(id)) {
            entityComponents = mComponents[id];
            if (entityComponents->Contains(entityId)) { return (T *) ((*entityComponents)[entityId]); }
        } else {
            entityComponents = AllocNew<FreeListMemory, EntityComponentMap>();
            mComponents.Set(id, entityComponents);
        }
        T *component = new(slab_alloc(getComponentSlab<T>())) T(std::forward<Args>(args)...);
        component->SetEntityId(entityId);
        entity->AddComponent(component);
        entityComponents->Set(entityId, component);
        return component;
    }

    inline void Commit(EntityId entityId) {
        assert(mEntities->Contains(entityId) && "ECS: entity not found");
        auto entity = (*mEntities)[entityId];
        for (auto sys: (*mSystems)) {
            sys.second->OnEntityCreated(entity);
        }
    }

    template<class T, class... Args>
    inline T *AddSystem(Args &&...args) {
        SystemId id = GetSystemTypeId<T>();
        if (mSystems->Contains(id)) return (T *) ((*mSystems)[id]);
        T *sys = new(slab_alloc(getSystemSlab<T>())) T(std::forward<Args>(args)...);
        sys->SetDirector(this);
        mSystems->Set(id, sys);
        return sys;
    }

    inline void Create() {
        for (auto sys: (*mSystems)) { sys.second->Create(); }
    }

    inline void Update() {

        debug_origin(vec2_zero);
        debug_color(color_yellow);
        float d = 8.0f;
        Vec3 pos = vec3_zero;
        debug_string3df(pos, " global(0) %d / %d", alloc->global->offset, alloc->global->size);
        pos.z -= d;
        debug_string3df(pos, " freelist(0) %d / %d", freelist_capacity(alloc->freelist), alloc->freelist->size);
        pos.z -= d;
        debug_string3df(pos, " entities(0) %d / %d -- %d|%d",
                        mEntitySlab->usage, mEntitySlab->capacity, mEntitySlab->bytes, mEntitySlab->objectSize);
        pos.z -= d;
        for (auto slab: mSystemsSlab) {
            debug_string3df(pos, "   system(%d) %d / %d -- %d|%d",
                            slab.first, slab.second->usage, slab.second->capacity, slab.second->bytes, slab.second->objectSize);
            pos.z -= d;
        }
        for (auto slab: mComponentsSlab) {
            debug_string3df(pos, "component(%d) %d / %d -- %d|%d",
                            slab.first, slab.second->usage, slab.second->capacity, slab.second->bytes, slab.second->objectSize);
            pos.z -= d;
        }
        debug_string3df(pos, " map - entities(%) %d", mEntities->Length());
        pos.z -= d;
        debug_string3df(pos, " map - systems(%) %d", mSystems->Length());
        pos.z -= d;
        debug_string3df(pos, " map - components(%) %d", mComponents.Length());
        pos.z -= d;
        for (auto p: mComponents) {
            debug_string3df(pos, " map - components(%d) %d", p.first, p.second->Length());
            pos.z -= d;
        }

        for (auto sys: (*mSystems)) {
            if (sys.second->mShouldUpdate)
                sys.second->Update();

            sys.second->Process();
        }

        while (!mDestroyStack.Empty()) {
            auto entityId = mDestroyStack.Pop();
            performDelete(entityId);
        }
    }
};


template<class... Components>
inline void System<Components...>::Process() {
    while (!mDestroyStack.Empty()) {
        auto entityId = mDestroyStack.Pop();
        if (mEntityIndex.Contains(entityId)) {
            auto index = mEntityIndex[entityId];
            const auto entityToMoveIndex = mComponents.Length() - 1;
            const auto movedEntity = std::get<0>(mComponents[entityToMoveIndex]);

            if (index < mComponents.Length()) {
                mComponents[index] = std::move(mComponents[entityToMoveIndex]);
                mComponents.Pop();

                auto movedEntityId = movedEntity->GetEntityId();
                if (mEntityIndex.Contains(movedEntityId)) {
                    mEntityIndex[movedEntityId] = index;
                    mShouldUpdate = mComponents.Length() > 0;
                }
            }
            mEntityIndex.Remove(entityId);
        }
    }

    while (!mCreateStack.Empty()) {
        auto entity = mCreateStack.Pop();
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
}
