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
#include "data/TArray.hpp"
#include "data/TFlatMap.hpp"
#include "data/TArrayStack.hpp"


template<class TAlloc>
class CEntity;

template<class TAlloc>
class CComponent;

template<class TAlloc>
class CBaseSystem;

template<class TAlloc>
class CDirector;

typedef size_t CECSIdType;

typedef CECSIdType CComponentId;
typedef CECSIdType CSystemId;
typedef CECSIdType CEntityId;

namespace ECSInternals {

    static inline CComponentId nextComponentId() {
        static CComponentId lastID{1};
        return lastID++;
    }

    static inline CSystemId nextSystemId() {
        static CSystemId lastID{1};
        return lastID++;
    }

    template<class TAlloc, class T>
    static inline CComponentId GetComponentTypeId() noexcept {
        static_assert(std::is_base_of_v<CComponent<TAlloc>, T>, "T must be a base class of Component");
        static CComponentId id{nextComponentId()};
        return id;
    }

    template<class TAlloc, class T>
    static inline CSystemId GetSystemTypeId() noexcept {
        static_assert(std::is_base_of_v<CBaseSystem<TAlloc>, T>, "T must be a base class of System");
        static CSystemId id{nextSystemId()};
        return id;
    }


    template<class TAlloc>
    inline void *ecs_global_alloc(size_t size) {
        return Alloc<TAlloc>(size);
    }

    template<class TAlloc>
    inline void ecs_global_free(void *ptr) {
        Free<TAlloc>((void **) &ptr);
    }

}

template<class TAlloc>
class CEntity {
    friend class CDirector<TAlloc>;

private:
    using ComponentMap = TFlatMap<CComponentId, CComponent<TAlloc> *, TAlloc>;
    ComponentMap mComponents;
    CEntityId mEntityId;

public:
    explicit inline CEntity(CEntityId id) : mEntityId(id) {}

    explicit inline CEntity(const CEntity &) = delete;

    virtual ~CEntity() = default;

    template<class T>
    inline void AddComponent(T *component) {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        if (mComponents.Contains(id)) {
            printf("Entity: Component %s already assigned to entity.\n", typeid(T).name());
            return;
        }
        mComponents.Set(id, component);
    }

    template<class T>
    inline T *GetComponent() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        if (!mComponents.Contains(id)) {
            printf("Entity: Component %s not found.\n", typeid(T).name());
            return nullptr;
        }
        return (T *) mComponents[id];
    }

    template<class T>
    inline bool HasComponent() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        return mComponents.Contains(id);
    }

    [[nodiscard]]
    inline ComponentMap &Components() { return mComponents; }

    [[nodiscard]]
    inline CEntityId Id() const { return mEntityId; }
};

template<class TAlloc>
class CComponent {
    friend class CDirector<TAlloc>;

public:
    CEntityId mEntityId{0};
    CEntity<TAlloc> *mEntity{nullptr};
    CDirector<TAlloc> *mDirector{nullptr};

    explicit inline CComponent() = default;

    explicit inline CComponent(const CComponent &) = delete;

    virtual ~CComponent() = default;

    virtual void Create() {};

    template<class T>
    inline void AddComponent(T *component) { mEntity->template AddComponent<T>(component); }

    template<class T>
    inline T *GetComponent() { return mEntity->template GetComponent<T>(); }

    template<class T>
    inline bool HasComponent() { return mEntity->template HasComponent<T>(); }

protected:
    inline void SetEntity(CDirector<TAlloc> *director, const CEntityId &id, CEntity<TAlloc> *entity) {
        mEntityId = id;
        mEntity = entity;
        mDirector = director;
        Create();
    }
};

template<class TAlloc>
class CBaseSystem {
public:
    explicit inline CBaseSystem() = default;

    explicit inline CBaseSystem(const CBaseSystem &) = delete;

    virtual ~CBaseSystem() = default;

    virtual void Create() {}

    virtual void Update() {}


protected:
    friend class CDirector<TAlloc>;

    using EntityIdStack = TArrayStack<CEntityId, TAlloc>;
    using EntityPtrStack = TArrayStack<CEntity<TAlloc> *, TAlloc>;
    using EntityIndexMap = TFlatMap<CEntityId, int, TAlloc>;

    EntityIdStack mDestroyStack;
    EntityPtrStack mCreateStack;
    EntityIndexMap mEntityIndex;
    CDirector<TAlloc> *mDirector{nullptr};
    bool mShouldUpdate = false;

    virtual void Process() = 0;

    virtual void Fit() {
        mDestroyStack.Fit();
        mCreateStack.Fit();
        mEntityIndex.Fit();
    }

    inline void OnEntityCreated(CEntity<TAlloc> *entity) {
        mCreateStack.Push(entity);
    }

    inline void OnEntityDestroyed(CEntityId entityId) {
        if (!mEntityIndex.Contains(entityId)) return;
        mDestroyStack.Push(entityId);
    }
};

template<class TAlloc, class ...Types>
class CSystem : public CBaseSystem<TAlloc> {
protected:
    using CTuple = std::tuple<std::add_pointer_t<Types>...>;
    TArray<CTuple, TAlloc> mComponents;

    template<int Index, class Type, class... Args>
    inline bool hasComponent(CComponentId id, CComponent<TAlloc> *component, CTuple &tuple) {
        if (ECSInternals::GetComponentTypeId<TAlloc, Type>() == id) {
            std::get<Index>(tuple) = static_cast<Type *>(component);
            return true;
        }
        return hasComponent<Index + 1, Args...>(id, component, tuple);
    }

    template<int Index>
    inline bool hasComponent([[maybe_unused]] CComponentId id, [[maybe_unused]]  CComponent<TAlloc> *component, [[maybe_unused]]  CTuple &tuple) {
        return false;
    }

    inline void Process() override;

    inline void Fit() override {
        CBaseSystem<TAlloc>::Fit();
        mComponents.Fit();
    }

public:
    explicit inline CSystem() = default;

    explicit inline CSystem(const CSystem &) = delete;

    template<class T>
    inline static T *Get(const CTuple &tuple) { return std::get<T *>(tuple); };

    inline TArray<CTuple, TAlloc> &Components() { return mComponents; }
};

template<class TAlloc>
class CDirector {
private:
    friend class CBaseSystem<TAlloc>;

    using EntityMap = TFlatMap<CEntityId, CEntity<TAlloc> *, TAlloc>;
    using SystemMap = TFlatMap<CSystemId, CBaseSystem<TAlloc> *, TAlloc>;
    using EntityComponentMap = TFlatMap<CEntityId, CComponent<TAlloc> *, TAlloc>;
    using ComponentEntityComponentMap = TFlatMap<CComponentId, EntityComponentMap *, TAlloc>;
    using EntityIdStack = TArrayStack<CEntityId, TAlloc>;
    using PartialSlabMemory = TFlatMap<CECSIdType, SlabMemory *, TAlloc>;


    CEntityId mEntityCounter{1};
    EntityMap mEntities;
    SystemMap mSystems;
    ComponentEntityComponentMap mComponents;

    SlabMemory *mEntitySlab{nullptr};
    PartialSlabMemory mComponentsSlab;
    PartialSlabMemory mSystemsSlab;
    EntityIdStack mDestroyStack;
    unsigned int mSlabCount = 16;


    template<class T>
    inline SlabMemory *makeSlab(int length) {
        GeneralAllocator pAlloc;
        pAlloc.alloc = ECSInternals::ecs_global_alloc<TAlloc>;
        pAlloc.free = ECSInternals::ecs_global_free<TAlloc>;
        return slab_create_alloc(pAlloc, sizeof(T) * length, sizeof(T));
    }

    template<class T>
    inline SlabMemory *getComponentSlab() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        SlabMemory *slab = makeSlab<T>(mSlabCount);
        mComponentsSlab.Set(id, slab);
        return slab;
    }

    template<class T>
    inline SlabMemory *getSystemSlab() {
        CSystemId id = ECSInternals::GetSystemTypeId<TAlloc, T>();
        if (mSystemsSlab.Contains(id)) return mSystemsSlab[id];
        SlabMemory *slab = makeSlab<T>(mSlabCount);
        mSystemsSlab.Set(id, slab);
        return slab;
    }

    inline SlabMemory *getComponentSlab(CComponentId id) {
        if (mComponentsSlab.Contains(id)) return mComponentsSlab[id];
        return nullptr;
    }

    inline void performDelete(CEntityId entityId) {
        if (!mEntities.Contains(entityId))
            return;
        CEntity<TAlloc> *entity = mEntities[entityId];

        for (auto p: mComponents) {
            CComponentId componentId = p.first;
            auto entityComponents = p.second;
            if (entityComponents->Contains(entityId)) {
                auto component = (*entityComponents)[entityId];
                component->~CComponent<TAlloc>();
                auto slab = getComponentSlab(componentId);
                slab_free(slab, (void **) &component);
                entityComponents->Remove(entityId);
            }
        }
        entity->~CEntity<TAlloc>();
        slab_free(mEntitySlab, (void **) &entity);
        mEntities.Remove(entityId);
    }

public:
    explicit inline CDirector() : CDirector(16) {}

    explicit inline CDirector(unsigned int slab) : mSlabCount{slab} {
        mEntitySlab = makeSlab<CEntity<TAlloc>>(mSlabCount);
    }


    explicit inline CDirector(const CDirector &) = delete;

    inline ~CDirector() {
        for (auto entity: mEntities) (entity.second)->~CEntity<TAlloc>();

        for (auto p: mComponents) {
            for (auto c: *(p.second))
                c.second->~CComponent<TAlloc>();
            Free<TAlloc>(&p.second);
        }

        for (auto entity: mSystems) (entity.second)->~CBaseSystem<TAlloc>();

        slab_destroy(&mEntitySlab);
        for (auto p: mComponentsSlab) slab_destroy(&(p.second));
        for (auto p: mSystemsSlab) slab_destroy(&(p.second));

    }

    inline CEntityId CreateEntity() {
        auto entity = new(slab_alloc(mEntitySlab)) CEntity<TAlloc>(mEntityCounter++);
        mEntities.Set(entity->Id(), entity);
        return entity->Id();
    }

    inline void DestroyEntity(CEntityId entityId) {
        if (!mEntities.Contains(entityId))
            return;
        auto entity = mEntities.Get(entityId);

        for (auto system: mSystems)
            system.second->OnEntityDestroyed(entityId);

        mDestroyStack.Push(entityId);
    }

    template<class T>
    inline EntityComponentMap *Query() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        if (!mComponents.Contains(id))
            return nullptr;
        return mComponents[id];
    }

    template<class T, class... Args>
    inline T *AddComponent(CEntityId entityId, Args &&...args) {
        if (!mEntities.Contains(entityId))
            return nullptr;
        EntityComponentMap *entityComponents;
        auto entity = mEntities[entityId];
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
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

    inline void Commit(CEntityId entityId) {
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

        slab_fit(mEntitySlab);
        for (auto p: mComponentsSlab) slab_fit(p.second);
        for (auto p: mSystemsSlab) slab_fit(p.second);
    }

    template<class T, class... Args>
    inline T *AddSystem(Args &&...args) {
        CSystemId id = ECSInternals::GetSystemTypeId<TAlloc, T>();
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

        debug_origin(vec2_zero);
        debug_stringf(Vec2{10, 100}, "entities: %d / %d", mEntities.Length(), mEntities.Capacity());

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
inline void CSystem<TAlloc, Types...>::Process() {
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
                    this->mEntityIndex.Set(movedEntityId, index);
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
