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
#include "data/TFastMap.hpp"
#include "data/TArray.hpp"
#include "data/TArrayStack.hpp"


template<class TAlloc>
class CEntity;

template<class TAlloc>
class CComponent;

template<class TAlloc>
class CBaseSystem;

template<class TAlloc>
class CDirector;

typedef uint64_t CECSIdType;

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
    using ComponentMap = TFastMap<CComponentId, CComponent<TAlloc> *, TAlloc>;
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
        const auto &component = mComponents.Get(id);
        if (component == nullptr) {
            printf("Entity: Component %s not found.\n", typeid(T).name());
            return nullptr;
        }
        return (T *) (*component);
    }

    template<class T>
    inline bool HasComponent() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        return mComponents.Contains(id);
    }

    inline ComponentMap *Components() { return &mComponents; }

    [[nodiscard]]
    inline const CEntityId &Id() const { return mEntityId; }
};

template<class TAlloc>
class CComponent {
    friend class CDirector<TAlloc>;

public:

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

    [[nodiscard]]
    inline const CEntityId &EntityId() const { return mEntityId; }

    inline const CEntity<TAlloc> *Entity() { return mEntity; }

    inline const CDirector<TAlloc> *Director() { return mDirector; }

protected:
    CEntityId mEntityId{0};
    CEntity<TAlloc> *mEntity{nullptr};
    CDirector<TAlloc> *mDirector{nullptr};

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
    using EntityIndexMap = TFastMap<CEntityId, int, TAlloc>;
    CDirector<TAlloc> *mDirector{nullptr};
    EntityIndexMap mEntityIndex;
    bool mShouldUpdate = false;
    bool mLocked = false;

    virtual void Fit() {
        mEntityIndex.Fit();
    }

    virtual void OnEntityCreated(CEntity<TAlloc> *entity) = 0;

    virtual void OnEntityDestroyed(CEntityId entityId) = 0;
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

    inline void Fit() override {
        CBaseSystem<TAlloc>::Fit();
        mComponents.Fit();
    }

    inline void SetTick(bool shouldUpdate) {
        this->mShouldUpdate = shouldUpdate;
        this->mLocked = !shouldUpdate;
    }

    inline void OnEntityCreated(CEntity<TAlloc> *entity) override {
        CTuple tuple;
        int matches = 0;
        for (const auto &component: *(entity->Components())) {
            if (hasComponent<0, Types...>(component->key, component->value, tuple)) {
                ++matches;
                if (matches == sizeof...(Types)) {
                    mComponents.Add(std::move(tuple));
                    this->mEntityIndex.Set(entity->Id(), mComponents.Length() - 1);
                    this->mShouldUpdate = mComponents.Length() > 0 && !this->mLocked;
                    break;
                }
            }
        }
    }

    inline void OnEntityDestroyed(CEntityId entityId) override {
        const auto &index = this->mEntityIndex.Get(entityId);
        if (index != nullptr) {
            const auto entityToMoveIndex = mComponents.Length() - 1;
            const auto movedEntity = std::get<0>(mComponents[entityToMoveIndex]);

            if (*index < mComponents.Length()) {
                mComponents[*index] = std::move(mComponents[entityToMoveIndex]);
                mComponents.Pop();

                const auto &movedEntityId = movedEntity->EntityId();
                const auto &movedEntityIndex = this->mEntityIndex.Get(movedEntityId);
                if (movedEntityIndex != nullptr) {
                    *movedEntityIndex = *index;
                    this->mShouldUpdate = mComponents.Length() > 0 && !this->mLocked;
                }
            }
            this->mEntityIndex.Remove(entityId);
        }
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

    using EntityMap = TFastMap<CEntityId, CEntity<TAlloc> *, TAlloc>;
    using SystemMap = TFastMap<CSystemId, CBaseSystem<TAlloc> *, TAlloc>;
    using EntityComponentMap = TFastMap<CEntityId, CComponent<TAlloc> *, TAlloc>;
    using ComponentEntityComponentMap = TFastMap<CComponentId, EntityComponentMap *, TAlloc>;
    using PartialSlabMemory = TFastMap<CECSIdType, SlabMemory *, TAlloc>;


    CEntityId mEntityCounter{1};
    EntityMap mEntities;
    SystemMap mSystems;
    ComponentEntityComponentMap mComponents;

    SlabMemory *mEntitySlab{nullptr};
    PartialSlabMemory mComponentsSlab;
    PartialSlabMemory mSystemsSlab;
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
        auto componentSlab = mComponentsSlab.Get(id);
        if (componentSlab != nullptr) return *componentSlab;
        SlabMemory *slab = makeSlab<T>(mSlabCount);
        mComponentsSlab.Set(id, slab);
        return slab;
    }

    template<class T>
    inline SlabMemory *getSystemSlab() {
        CSystemId id = ECSInternals::GetSystemTypeId<TAlloc, T>();
        auto systemSlab = mSystemsSlab.Get(id);
        if (systemSlab != nullptr) return *systemSlab;
        SlabMemory *slab = makeSlab<T>(mSlabCount);
        mSystemsSlab.Set(id, slab);
        return slab;
    }

    inline SlabMemory *getComponentSlab(CComponentId id) {
        const auto &slab = mComponentsSlab.Get(id);
        if (slab != nullptr) return *slab;
        return nullptr;
    }

    inline void performDelete(CEntityId entityId) {
        const auto &entity = mEntities.Get(entityId);

        if (entity == nullptr)
            return;

        for (const auto &p: mComponents) {
            CComponentId componentId = p->key;
            const auto &component = p->value->Get(entityId);
            if (component != nullptr) {
                (*component)->~CComponent<TAlloc>();
                auto slab = getComponentSlab(componentId);
                slab_free(slab, (void **) component);
                p->value->Remove(entityId);
            }
        }
        (*entity)->~CEntity<TAlloc>();
        slab_free(mEntitySlab, (void **) entity);
        mEntities.Remove(entityId);
    }

public:
    explicit inline CDirector() : CDirector(16) {}

    explicit inline CDirector(unsigned int slab) : mSlabCount{slab} {
        mEntitySlab = makeSlab<CEntity<TAlloc>>(mSlabCount);
    }


    explicit inline CDirector(const CDirector &) = delete;

    inline ~CDirector() {
        for (const auto &entity: mEntities) (entity->value)->~CEntity<TAlloc>();

        for (const auto &p: mComponents) {
            for (const auto &c: *(p->value))
                c->value->~CComponent<TAlloc>();
            Free<TAlloc>(&p->value);
        }

        for (const auto &entity: mSystems) (entity->value)->~CBaseSystem<TAlloc>();

        slab_destroy(&mEntitySlab);
        for (const auto &p: mComponentsSlab) slab_destroy(&(p->value));
        for (const auto &p: mSystemsSlab) slab_destroy(&(p->value));

    }

    inline CEntityId CreateEntity() {
        auto entity = new(slab_alloc(mEntitySlab)) CEntity<TAlloc>(mEntityCounter++);
        mEntities.Set(entity->Id(), entity);
        return entity->Id();
    }

    inline void DestroyEntity(CEntityId entityId) {
        if (!mEntities.Contains(entityId))
            return;

        for (auto system: mSystems)
            system->value->OnEntityDestroyed(entityId);

        performDelete(entityId);
    }

    template<class T>
    inline EntityComponentMap *Query() {
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        const auto &found = mComponents.Get(id);
        if (found == nullptr) return nullptr;
        return (*found);
    }

    template<class T, class... Args>
    inline T *AddComponent(CEntityId entityId, Args &&...args) {
        const auto &entity = mEntities.Get(entityId);
        if (entity == nullptr) return nullptr;
        EntityComponentMap *entityComponents;
        CComponentId id = ECSInternals::GetComponentTypeId<TAlloc, T>();
        const auto &found = mComponents.Get(id);
        if (found != nullptr) {
            entityComponents = (*found);
            const auto &component = entityComponents->Get(entityId);
            if (component != nullptr)
                return (T *) (*component);
        } else {
            entityComponents = AllocNew<TAlloc, EntityComponentMap>();
            mComponents.Set(id, entityComponents);
        }
        T *component = new(slab_alloc(getComponentSlab<T>())) T(std::forward<Args>(args)...);
        component->SetEntity(this, entityId, (*entity));
        (*entity)->template AddComponent<T>(component);
        entityComponents->Set(entityId, component);
        return component;
    }

    inline void Commit(CEntityId entityId) {
        const auto &entity = mEntities.Get(entityId);
        assert(entity != nullptr && "ECS: entity not found");
        for (const auto &sys: mSystems) {
            sys->value->OnEntityCreated(*entity);
        }
    }

    inline void Fit() {
        for (const auto &entity: mEntities) entity->value->mComponents.Fit();
        mEntities.Fit();
        for (const auto &sys: mSystems) {
            sys->value->Fit();
        }
        mSystems.Fit();
        for (const auto &p: mComponents) p->value->Fit();
        mComponents.Fit();

        slab_fit(mEntitySlab);
        for (const auto &p: mComponentsSlab) slab_fit(p->value);
        for (const auto &p: mSystemsSlab) slab_fit(p->value);
    }

    template<class T, class... Args>
    inline T *AddSystem(Args &&...args) {
        CSystemId id = ECSInternals::GetSystemTypeId<TAlloc, T>();
        const auto &system = mSystems.Get(id);
        if (system != nullptr) return (T *) (*system);
        T *sys = new(slab_alloc(getSystemSlab<T>())) T(std::forward<Args>(args)...);
        sys->mDirector = this;
        mSystems.Set(id, sys);
        return sys;
    }

    inline void Create() {
        for (auto sys: mSystems) { sys->value->Create(); }
    }

    inline void Update() {
        debug_origin(vec2_zero);
        debug_stringf(Vec2{10, 100}, "entities: %d / %d", mEntities.Length(), mEntities.Capacity());

        for (auto sys: mSystems) {
            if (sys->value->mShouldUpdate)
                sys->value->Update();
        }
    }
};
