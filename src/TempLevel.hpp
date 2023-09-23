#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"

}

#include "data/Heap.hpp"
#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


class CustomTempAllocator {
public:
    static BuddyMemory *buddy;

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return buddy_alloc(buddy, size);
    }

    inline static void Free(void **ptr) {
        buddy_free(buddy, ptr);
    }
};

BuddyMemory *CustomTempAllocator::buddy = nullptr;

class TempLevel : public Level {
    using TAlloc = CustomTempAllocator;

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
        float timeSpan{1.0f};
        float blend{1.0f};
        float size{12.0f};
        float damping{0};

        explicit inline ParticleComponent() = default;
    };

    struct EmitterComponent : public Component<TAlloc> {
        float delay{0.01f};
        float lastSpawn{0};
        Color color1{};
        Color color2{};
        int maxParticles{40};
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
                        Vec3 rnd = vec3_add(vec3_rand(1, 1, 5), vec3(0,0, 5));
                        particle->size = 12.0f;
                        particle->emitter = pEmitter;
                        particle->position = pTransform->position;
                        particle->velocity = vec3_mulf(rnd, 0.5f);
                        particle->spawnTime = gameTime->time;
                        particle->damping = (randf() * 0.02f) + 0.95f;
                        pEmitter->lastSpawn = gameTime->time;
                    }
                    act += pEmitter->activeParticles;
                }

            }
            debug_string3df(vec3_zero, "%d", act);
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
                    if(pParticle->emitter->removeParticles == pParticle->emitter->maxParticles)
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
                fill_cubef(pParticle->position, pParticle->size, c);
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
            dest = vec3_snap(dest, 5.0f);

            if (input_mousepress(MOUSE_LEFT)) {

                auto entity = mDirector->CreateEntity();
                mDirector->AddComponent<TransformComponent>(entity, dest);
                auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
                Color c = color_rand();
                emitterComponent->color1 = color_darken(c, 0.1f);
                emitterComponent->color2 = color_lighten(c, 0.5f);
                mDirector->Commit(entity);
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

            draw_cubef(dest, 10.0f, color_red);
        }
    };

public:
    Director<TAlloc> *mDirector{nullptr};


    inline void Create() override {
        CustomTempAllocator::buddy = make_buddy(24);
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
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d", CustomTempAllocator::buddy->usage, 1<< CustomTempAllocator::buddy->order);

    }

    inline void Destroy() override {
        Free<TAlloc>(&mDirector);
        buddy_destroy(&CustomTempAllocator::buddy);
    }
};