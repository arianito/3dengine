#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
#include "mem/p2slab.h"

}

#include "data/Heap.hpp"
#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


class CustomTempAllocator {
public:
    static BuddyMemory *buddy;

    static inline void create() {
        buddy = make_buddy(24);

    }

    static inline void destroy() {
        buddy_destroy(&buddy);
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return buddy_alloc(buddy, size);
    }

    inline static void Free(void **ptr) {
        buddy_free(buddy, ptr);
    }

    inline static size_t usage() {
        return buddy->usage;
    }

    inline static size_t size() {
        return 1 << CustomTempAllocator::buddy->order;
    }
};

class CustomTempAllocator2 {
public:
    static FreeListMemory *freelist;

    static inline void create() {
        freelist = make_freelist(16 * MEGABYTES);

    }

    static inline void destroy() {
        freelist_destroy(&freelist);
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return freelist_alloc(freelist, size, alignment);
    }

    inline static void Free(void **ptr) {
        freelist_free(freelist, ptr);
    }

    inline static size_t usage() {
        return freelist_capacity(freelist);
    }

    inline static size_t size() {
        return freelist->size;
    }
};


class CustomTempAllocator3 {
public:
    static P2SlabMemory *slab;

    static inline void create() {
        slab = make_p2slab(20);

    }

    static inline void destroy() {
        p2slab_destroy(&slab);
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return p2slab_alloc(slab, size);
    }

    inline static void Free(void **ptr) {
        p2slab_free(slab, ptr);
    }

    inline static size_t usage() {
        return slab->usage;
    }

    inline static size_t size() {
        return slab->total;
    }
};

class CustomTempAllocator4 {
public:
    static size_t used;

    static inline void create() {

    }

    static inline void destroy() {
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        used += size;
        return ::operator new( size);
    }

    inline static void Free(void **ptr) {
        ::operator delete(*ptr);
    }

    inline static size_t usage() {
        return 0;
    }

    inline static size_t size() {
        return used;
    }
};
BuddyMemory *CustomTempAllocator::buddy = nullptr;
FreeListMemory *CustomTempAllocator2::freelist = nullptr;
P2SlabMemory *CustomTempAllocator3::slab = nullptr;
size_t CustomTempAllocator4::used = 0;

class TempLevel : public Level {
    using TAlloc = CustomTempAllocator3;

    struct TransformComponent : public Component<TAlloc> {
        Vec3 position;

        explicit inline TransformComponent(Vec3 position) : position(position) {}
    };

    struct MovementComponent : public Component<TAlloc> {
        Vec3 destination{0, 0, 0};
        float speed{20.0f};


        inline void Create() override {
            auto t = GetComponent<TransformComponent>();
            if (!t)return;
            destination = t->position;
        }

        explicit inline MovementComponent(float speed) : speed(speed) {}

    };

    struct ShapeComponent : public Component<TAlloc> {
        explicit inline ShapeComponent() = default;
    };

    struct EmitterComponent;

    struct ParticleComponent : public Component<TAlloc> {
        EmitterComponent *emitter{nullptr};
        Vec3 position{0.0f, 0.0f, 0.0f};
        Vec3 velocity{0.0f, 0.0f, 0.0f};
        float spawnTime{0.0f};
        float timeSpan{2.0f};
        float blend{1.0f};
        float size{12.0f};
        float damping{0};
    };

    struct EmitterComponent : public Component<TAlloc> {
        float delay{0.01f};
        float lastSpawn{0};
        Color color1{};
        Color color2{};
        int maxParticles{120};
        int activeParticles{0};
        int removeParticles{0};

        explicit inline EmitterComponent() = default;
    };

    struct RenderSystem : public System<TAlloc, ShapeComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                draw_circleXY(pTransform->position, 10, color_yellow, 12);
            }
        }
    };

    struct EmitterSystem : public System<TAlloc, EmitterComponent, TransformComponent> {
        inline void Update() override {
            int act = 0;
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                auto pEmitter = Get<EmitterComponent>(bucket);
                if ((gameTime->time - pEmitter->lastSpawn) > pEmitter->delay) {

                    ParticleComponent *particle{nullptr};
                    if (pEmitter->activeParticles < pEmitter->maxParticles) {
                        auto entityId = mDirector->CreateEntity();
                        particle = mDirector->AddComponent<ParticleComponent>(entityId);
                        mDirector->Commit(entityId);
                        pEmitter->activeParticles++;
                    }

                    if (particle) {
                        Vec3 rnd = vec3_add(vec3_rand(1, 1, 5), vec3(0, 0, 5));
                        particle->size = 24.0f;
                        particle->emitter = pEmitter;
                        particle->position = pTransform->position;
                        particle->velocity = vec3_mulf(rnd, 0.5f);
                        particle->spawnTime = gameTime->time;
                        particle->damping = (randf() * 0.1f) + 0.85f;
                        pEmitter->lastSpawn = gameTime->time;
                    }
                }

            }
        }
    };

    struct ParticleSystem : public System<TAlloc, ParticleComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pParticle = Get<ParticleComponent>(bucket);
                float t = (gameTime->time - pParticle->spawnTime) / pParticle->timeSpan;
                pParticle->blend = 1 - t;
                pParticle->velocity = vec3_mulf(pParticle->velocity, pParticle->damping);
                pParticle->position = vec3_add(pParticle->position, pParticle->velocity);
                pParticle->size = pParticle->size * pParticle->damping;

                if ((t > 1 || pParticle->size < 0.5f)) {
                    mDirector->DestroyEntity(pParticle->mEntityId);
                    pParticle->emitter->removeParticles++;
                    if (pParticle->emitter->removeParticles == pParticle->emitter->maxParticles)
                        mDirector->DestroyEntity(pParticle->emitter->mEntityId);

                }
            }
        }
    };

    struct MovementSystem : public System<TAlloc, MovementComponent, TransformComponent> {
        inline void Update() override {
            Ray r = camera_screenToWorld(input->position);
            Vec3 dest = vec3_intersectPlane(r.origin, vec3_add(r.origin, r.direction), vec3_zero, vec3_up);
            int down = input_mousepress(MOUSE_LEFT);
            float i = gameTime->time * 200.0f;
            float n = (float) Components().Length();
            float sp = 360.0f / n;
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                auto pMovement = Get<MovementComponent>(bucket);
                if (down) {
                    Vec3 rnd = vec3(cosd(i), sind(i), 0);
                    pMovement->destination = vec3_add(dest, vec3_mulf(rnd, 30.0f));
                }
                pTransform->position = vec3_moveTowards(pTransform->position, pMovement->destination, pMovement->speed * gameTime->deltaTime);
                i += sp;
            }
        }
    };

    struct ParticleRenderSystem : public System<TAlloc, ParticleComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pParticle = Get<ParticleComponent>(bucket);
                Color c = color_lerp01(pParticle->emitter->color2, pParticle->emitter->color1, powf(pParticle->blend, 12));
                draw_point(pParticle->position, pParticle->size, c);
            }
        }
    };

    struct DummyComponent : Component<TAlloc> {
    };

    struct SpawnSystem : public System<TAlloc, DummyComponent> {
        inline void Create() override {
            mShouldUpdate = true;
        }

        inline void Update() override {
            Ray r = camera_screenToWorld(input->position);
            Vec3 dest = vec3_intersectPlane(r.origin, vec3_add(r.origin, r.direction), vec3_zero, vec3_up);

            if (input_mousedown(MOUSE_LEFT)) {

                for (int i = 0; i < 20; i++) {
                    Vec3 pos = vec3_add(dest, vec3_rand(100, 100, 0));
                    pos = vec3_snapCube(pos, 10.0f);
                    auto entity = mDirector->CreateEntity();
                    mDirector->AddComponent<TransformComponent>(entity, pos);
                    auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
                    Color c = color_rand();
                    emitterComponent->color1 = color_darken(c, 0.1f);
                    emitterComponent->color2 = color_lighten(c, 0.5f);
                    mDirector->Commit(entity);
                }
            }
            if (input_keydown(KEY_SPACE)) {
                auto entity = mDirector->Query<EmitterComponent>();
                if (entity != nullptr) {
                    for (const auto p: *entity) {
                        mDirector->DestroyEntity(p.first);
                        break;
                    }
                }
            }

            dest = vec3_snapCube(dest, 10.0f);
            draw_cubef(dest, 10.0f, color_red);
        }
    };

public:
    Director<TAlloc> *mDirector{nullptr};


    inline void Create() override {
        TAlloc::create();
        mDirector = AllocNew<TAlloc, Director<TAlloc>>();
        mDirector->AddSystem<MovementSystem>();
        mDirector->AddSystem<RenderSystem>();
        mDirector->AddSystem<EmitterSystem>();
        mDirector->AddSystem<ParticleSystem>();
        mDirector->AddSystem<ParticleRenderSystem>();
        mDirector->AddSystem<SpawnSystem>();
        mDirector->Create();
    }

    inline void Update() override {
        mDirector->Update();
        debug_origin(vec2(1, 1));
        debug_color(color_white);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos,
                      "Alloc: %.2f",
                      (float)TAlloc::usage() / (float )TAlloc::size()
        );
    }

    inline void Destroy() override {
        Free<TAlloc>(&mDirector);
        TAlloc::destroy();
    }
};