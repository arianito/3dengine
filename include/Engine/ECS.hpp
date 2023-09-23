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


template<class TAlloc>
class Entity;

template<class TAlloc>
class Component;

template<class TAlloc>
class BaseSystem;

template<class TAlloc>
class Director;


typedef size_t BaseType;
typedef BaseType ComponentId;
typedef BaseType SystemId;
typedef BaseType EntityId;


static inline ComponentId nextComponentId() {
    static ComponentId lastID{1};
    return lastID++;
}

static inline SystemId nextSystemId() {
    static SystemId lastID{1};
    return lastID++;
}

template<class TAlloc, class T>
static inline ComponentId GetComponentTypeId() noexcept {
    static_assert(std::is_base_of_v<Component<TAlloc>, T>, "T must be a base class of Component");
    static ComponentId id{nextComponentId()};
    return id;
}

template<class TAlloc, class T>
static inline SystemId GetSystemTypeId() noexcept {
    static_assert(std::is_base_of_v<BaseSystem<TAlloc>, T>, "T must be a base class of System");
    static SystemId id{nextSystemId()};
    return id;
}

template<class TAlloc>
class Entity {
    friend class Director<TAlloc>;

private:
    using ComponentMap = ProbeHashTable<ComponentId, Component<TAlloc> *, TAlloc>;
    ComponentMap mComponents;
    EntityId mEntityId;

public:
    explicit inline Entity(EntityId id) : mEntityId(id) {}

    explicit inline Entity(const Entity &) = delete;

    virtual ~Entity() = default;

    template<class T>
    inline void AddComponent(T *component) {
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        if (mComponents.Contains(id)) {
            printf("Entity: Component %s already assigned to entity.\n", typeid(T).name());
            return;
        }
        mComponents.Set(id, component);
    }

    template<class T>
    inline T *GetComponent() {
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        if (!mComponents.Contains(id)) {
            printf("Entity: Component %s not found.\n", typeid(T).name());
            return nullptr;
        }
        return (T *) mComponents[id];
    }

    template<class T>
    inline bool HasComponent() {
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        return mComponents.Contains(id);
    }

    [[nodiscard]] inline ComponentMap &Components() { return mComponents; }

    [[nodiscard]] inline EntityId Id() const { return mEntityId; }
};

template<class TAlloc>
class Component {
    friend class Director<TAlloc>;

public:
    EntityId mEntityId{0};
    Entity<TAlloc> *mEntity{nullptr};
    Director<TAlloc> *mDirector{nullptr};

    explicit inline Component() = default;

    explicit inline Component(const Component &) = delete;

    virtual ~Component() = default;

    virtual void Create() {};

    template<class T>
    inline void AddComponent(T *component) { mEntity->template AddComponent<T>(component); }

    template<class T>
    inline T *GetComponent() { return mEntity->template GetComponent<T>(); }

    template<class T>
    inline bool HasComponent() { return mEntity->template HasComponent<T>(); }

protected:
    inline void SetEntity(Director<TAlloc> *director, const EntityId &id, Entity<TAlloc> *entity) {
        mEntityId = id;
        mEntity = entity;
        mDirector = director;
        Create();
    }
};

template<class TAlloc>
class BaseSystem {
public:
    explicit inline BaseSystem() = default;

    explicit inline BaseSystem(const BaseSystem &) = delete;

    virtual ~BaseSystem() = default;

    virtual void Create() {}

    virtual void Update() {}


protected:
    friend class Director<TAlloc>;

    using EntityIdStack = FixedStack<EntityId, TAlloc>;
    using EntityPtrStack = FixedStack<Entity<TAlloc> *, TAlloc>;
    using EntityIndexMap = ProbeHashTable<EntityId, int, TAlloc>;

    EntityIdStack mDestroyStack;
    EntityPtrStack mCreateStack;
    EntityIndexMap mEntityIndex;
    Director<TAlloc> *mDirector{nullptr};
    bool mShouldUpdate = false;

    virtual void Process() = 0;

    virtual void Fit() {
        mDestroyStack.Fit();
        mCreateStack.Fit();
        mEntityIndex.Fit();
    }

    inline void OnEntityCreated(Entity<TAlloc> *entity) {
        mCreateStack.Push(entity);
    }

    inline void OnEntityDestroyed(EntityId entityId) {
        if (!mEntityIndex.Contains(entityId)) return;
        mDestroyStack.Push(entityId);
    }
};


template<class TAlloc, class ...Types>
class System : public BaseSystem<TAlloc> {
protected:
    using CTuple = std::tuple<std::add_pointer_t<Types>...>;
    Array<CTuple, TAlloc> mComponents;

    template<int Index, class Type, class... Args>
    inline bool hasComponent(ComponentId id, Component<TAlloc> *component, CTuple &tuple) {
        if (GetComponentTypeId<TAlloc, Type>() == id) {
            std::get<Index>(tuple) = static_cast<Type *>(component);
            return true;
        }
        return hasComponent<Index + 1, Args...>(id, component, tuple);
    }

    template<int Index>
    inline bool hasComponent([[maybe_unused]] ComponentId id, [[maybe_unused]]  Component<TAlloc> *component, [[maybe_unused]]  CTuple &tuple) {
        return false;
    }

    inline void Process() override;

    inline void Fit() override {
        BaseSystem<TAlloc>::Fit();
        mComponents.Fit();
    }

public:
    explicit inline System() = default;

    explicit inline System(const System &) = delete;

    template<class T>
    inline static T *Get(const CTuple &tuple) { return std::get<T *>(tuple); };

    inline Array<CTuple, TAlloc> &Components() { return mComponents; }
};

template<class TAlloc>
class Director {
private:
    friend class BaseSystem<TAlloc>;

    using EntityMap = ProbeHashTable<EntityId, Entity<TAlloc> *, TAlloc>;
    using SystemMap = ProbeHashTable<SystemId, BaseSystem<TAlloc> *, TAlloc>;
    using EntityComponentMap = ProbeHashTable<EntityId, Component<TAlloc> *, TAlloc>;
    using ComponentEntityComponentMap = ProbeHashTable<ComponentId, EntityComponentMap *, TAlloc>;
    using EntityIdStack = FixedStack<EntityId, TAlloc>;
    using PartialSlabMemory = ProbeHashTable<BaseType, SlabMemory *, TAlloc>;

    EntityId mEntityCounter{1};
    EntityMap mEntities;
    SystemMap mSystems;
    ComponentEntityComponentMap mComponents;

    SlabMemory *mEntitySlab;
    PartialSlabMemory mComponentsSlab;
    PartialSlabMemory mSystemsSlab;
    EntityIdStack mDestroyStack;

    inline static void *ecs_global_alloc(size_t size) {
        return Alloc<TAlloc>(size, sizeof(size_t));
    }

    inline static void ecs_global_free(void *ptr) {
        Free<TAlloc>(&ptr);
    }

    template<class T>
    inline SlabMemory *makeSlab(int length) {
        GeneralAllocator pAlloc;
        pAlloc.alloc = ecs_global_alloc;
        pAlloc.free = ecs_global_free;
        return slab_create_alloc(pAlloc, sizeof(T) * length, sizeof(T));
    }

    template<class T>
    inline SlabMemory *getComponentSlab() {
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        SlabMemory *slab = makeSlab<T>(16);
        mComponentsSlab.Set(id, slab);
        return slab;
    }

    template<class T>
    inline SlabMemory *getSystemSlab() {
        SystemId id = GetSystemTypeId<TAlloc, T>();
        if (mSystemsSlab.Contains(id)) return mSystemsSlab[id];
        SlabMemory *slab = makeSlab<T>(16);
        mSystemsSlab.Set(id, slab);
        return slab;
    }

    inline SlabMemory *getComponentSlab(ComponentId id) {
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        return nullptr;
    }

    inline void performDelete(EntityId entityId) {
        if (!mEntities.Contains(entityId))
            return;
        Entity<TAlloc> *entity = mEntities[entityId];

        for (auto p: mComponents) {
            ComponentId componentId = p.first;
            auto entityComponents = p.second;
            if (entityComponents->Contains(entityId)) {
                auto component = (*entityComponents)[entityId];
                component->~Component<TAlloc>();
                auto slab = getComponentSlab(componentId);
                slab_free(slab, (void **) &component);
                entityComponents->Remove(entityId);
            }
        }
        entity->~Entity<TAlloc>();
        slab_free(mEntitySlab, (void **) &entity);
        mEntities.Remove(entityId);
    }

public:
    explicit inline Director() {
        mEntitySlab = makeSlab<Entity<TAlloc>>(16);
    }

    explicit inline Director(const Director &) = delete;

    inline ~Director() {
        for (auto entity: mEntities) (entity.second)->~Entity<TAlloc>();

        for (auto p: mComponents) {
            for (auto c: *(p.second))
                c.second->~Component<TAlloc>();
            Free<TAlloc>(&p.second);
        }

        for (auto entity: mSystems) (entity.second)->~BaseSystem<TAlloc>();

        slab_destroy(&mEntitySlab);
        for (auto p: mComponentsSlab) slab_destroy(&(p.second));
        for (auto p: mSystemsSlab) slab_destroy(&(p.second));

    }

    inline EntityId CreateEntity() {
        auto entity = new(slab_alloc(mEntitySlab)) Entity<TAlloc>(mEntityCounter++);
        mEntities.Set(entity->Id(), entity);
        return entity->Id();
    }

    inline void DestroyEntity(EntityId entityId) {
        if (!mEntities.Contains(entityId))
            return;

        auto entity = mEntities[entityId];

        for (auto system: mSystems)
            system.second->OnEntityDestroyed(entityId);

        mDestroyStack.Push(entityId);
    }

    template<class T>
    inline EntityComponentMap *Query() {
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        if (!mComponents.Contains(id))
            return nullptr;
        return mComponents[id];
    }

    template<class T, class... Args>
    inline T *AddComponent(EntityId entityId, Args &&...args) {
        if (!mEntities.Contains(entityId))
            return nullptr;
        EntityComponentMap *entityComponents;
        auto entity = mEntities[entityId];
        ComponentId id = GetComponentTypeId<TAlloc, T>();
        if (mComponents.Contains(id)) {
            entityComponents = mComponents[id];
            if (entityComponents->Contains(entityId)) { return (T *) ((*entityComponents)[entityId]); }
        } else {
            entityComponents = AllocNew<TAlloc, EntityComponentMap>();
            mComponents.Set(id, entityComponents);
        }
        T *component = new(slab_alloc(getComponentSlab<T>())) T(std::forward<Args>(args)...);
        component->SetEntity(this, entityId, entity);
        entity->template AddComponent<T>(component);
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

    inline void Fit() {
        for (auto entity: mEntities) (entity.second)->mComponents.Fit();
        mEntities.Fit();
        for (auto sys: mSystems) {
            sys.second->Fit();
        }
        mSystems.Fit();
        for (auto p: mComponents) p.second->Fit();
        mComponents.Fit();
    }

    template<class T, class... Args>
    inline T *AddSystem(Args &&...args) {
        SystemId id = GetSystemTypeId<TAlloc, T>();
        if (mSystems.Contains(id)) return (T *) (mSystems[id]);
        T *sys = new(slab_alloc(getSystemSlab<T>())) T(std::forward<Args>(args)...);
        sys->mDirector = this;
        mSystems.Set(id, sys);
        return sys;
    }

    inline void Create() {
        for (auto sys: mSystems) { sys.second->Create(); }
    }

    inline void Update() {
        for (auto sys: mSystems) {
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


template<class TAlloc, class... Types>
inline void System<TAlloc, Types...>::Process() {
    while (!this->mDestroyStack.Empty()) {
        auto entityId = this->mDestroyStack.Pop();
        if (this->mEntityIndex.Contains(entityId)) {
            auto index = this->mEntityIndex[entityId];
            const auto entityToMoveIndex = mComponents.Length() - 1;
            const auto movedEntity = std::get<0>(mComponents[entityToMoveIndex]);

            if (index < mComponents.Length()) {
                mComponents[index] = std::move(mComponents[entityToMoveIndex]);
                mComponents.Pop();

                auto movedEntityId = movedEntity->mEntityId;
                if (this->mEntityIndex.Contains(movedEntityId)) {
                    this->mEntityIndex[movedEntityId] = index;
                    this->mShouldUpdate = mComponents.Length() > 0;
                }
            }
            this->mEntityIndex.Remove(entityId);
        }
    }

    while (!this->mCreateStack.Empty()) {
        auto entity = this->mCreateStack.Pop();
        CTuple tuple;
        int matches = 0;
        for (auto component: entity->Components()) {
            if (hasComponent<0, Types...>(component.first, component.second, tuple)) {
                ++matches;
                if (matches == sizeof...(Types)) {
                    mComponents.Add(std::move(tuple));
                    this->mEntityIndex.Set(entity->Id(), mComponents.Length() - 1);
                    this->mShouldUpdate = mComponents.Length() > 0;
                    break;
                }
            }
        }
    }
}
