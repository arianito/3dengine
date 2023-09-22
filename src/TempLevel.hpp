#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"

class TempLevel : public Level {

    struct TransformComponent : public Component {
        Vec3 position;

        explicit inline TransformComponent(Vec3 position) : position(position) {}
    };

    struct MovementComponent : public Component {
        Vec3 destination{0, 0, 0};
        float speed{20.0f};


        inline void Create() override {
            auto t = GetComponent<TransformComponent>();

            if (t) {
                destination = t->position;
            }
        }

        explicit inline MovementComponent(float speed) : speed(speed) {}

    };

    struct ShapeComponent : public Component {
        explicit inline ShapeComponent() = default;
    };

    struct EmitterComponent : public Component {
        float delay{0.01f};
        float lastSpawn{0};
        int activeParticles{0};
        Color color1{};
        Color color2{};

        explicit inline EmitterComponent() = default;
    };

    struct ParticleComponent : public Component {
        EmitterComponent *emitter{nullptr};

        Vec3 position{0.0f, 0.0f, 0.0f};
        Vec3 velocity{0.0f, 0.0f, 0.0f};
        float spawnTime{0.0f};
        float timeSpan{2.0f};
        float blend{1.0f};
        float size{8.0f};
        float damping{0};

        explicit inline ParticleComponent() = default;
    };


    struct RenderSystem : public System<ShapeComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                draw_circleXY(pTransform->position, 10, color_yellow, 12);
            }
        }
    };

    struct MovementSystem : public System<MovementComponent, TransformComponent> {
        inline void Update() override {
            Ray r = camera_screenToWorld(input->position);
            Vec3 dest = vec3_intersectPlane(r.origin, vec3_add(r.origin, r.direction), vec3_zero, vec3_up);
            int down = input_mousepress(MOUSE_LEFT);
            float i = gameTime->time * 200.0f;
            float n = (float)Components().Length();
            float sp = 360.0f / n;
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                auto pMovement = Get<MovementComponent>(bucket);
                if (down) {
//                    Vec3 rnd = vec3(randf() * 1.0f - 0.5f, randf() * 1.0f - 0.5f, 0);
                    Vec3 rnd = vec3(cosd(i), sind(i), 0);
                    pMovement->destination = vec3_add(dest, vec3_mulf(rnd, 30.0f));
                }
                pTransform->position = vec3_moveTowards(pTransform->position, pMovement->destination, pMovement->speed * gameTime->deltaTime);
                i+= sp;
            }
        }
    };

    struct EmitterSystem : public System<EmitterComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pTransform = Get<TransformComponent>(bucket);
                auto pEmitter = Get<EmitterComponent>(bucket);
                if ((gameTime->time - pEmitter->lastSpawn) > pEmitter->delay) {
                    auto director = GetDirector();

                    auto entityId = director->CreateEntity();
                    auto particle = director->AddComponent<ParticleComponent>(entityId);
                    particle->emitter = pEmitter;
                    Vec3 rnd = vec3(randf() * 1.0f - 0.5f, randf() * 1.0f - 0.5f, randf() * 3.0f + 1.0f);
                    particle->position = pTransform->position;
                    particle->velocity = vec3_mulf(rnd, 0.5f);
                    particle->spawnTime = gameTime->time;
                    particle->damping = (randf() * 0.02f) + 0.95f;
                    director->Commit(entityId);
                    pEmitter->activeParticles++;
                    pEmitter->lastSpawn = gameTime->time;
                }

            }
        }
    };

    struct ParticleSystem : public System<ParticleComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pParticle = Get<ParticleComponent>(bucket);
                float t = (gameTime->time - pParticle->spawnTime) / pParticle->timeSpan;
                pParticle->blend = 1 - t;
                pParticle->velocity = vec3_mulf(pParticle->velocity, pParticle->damping);
                pParticle->position = vec3_add(pParticle->position, pParticle->velocity);
                pParticle->size = pParticle->size * pParticle->damping;

                if (t > 1 || pParticle->size < 0.5f) {
                    pParticle->emitter->activeParticles--;
                    GetDirector()->DestroyEntity(pParticle->Id());
                }
            }
        }
    };

    struct ParticleRenderSystem : public System<ParticleComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pParticle = Get<ParticleComponent>(bucket);
                Color c = color_lerp01(pParticle->emitter->color2, pParticle->emitter->color1, powf(pParticle->blend, 12));
                fill_cubef(pParticle->position, pParticle->size, c);
            }
        }
    };

public:
    Director *mDirector{nullptr};


    inline void Create() override {
        mDirector = AllocNew<BuddyMemory, Director>();
        mDirector->AddSystem<MovementSystem>();
        mDirector->AddSystem<RenderSystem>();

        mDirector->AddSystem<EmitterSystem>();
        mDirector->AddSystem<ParticleSystem>();
        mDirector->AddSystem<ParticleRenderSystem>();

        {
            auto entity = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(entity, vec3(-100, 0, 0));
            auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
            emitterComponent->color1 = color_darkBlue;
            emitterComponent->color2 = color_lightBlue;
            mDirector->AddComponent<MovementComponent>(entity, 200.0f);
            mDirector->Commit(entity);
        }

        {
            auto entity = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(entity, vec3(100, 0, 0));
            auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
            emitterComponent->color1 = color_gray;
            emitterComponent->color2 = color_white;
            mDirector->AddComponent<MovementComponent>(entity, 200.0f);
            mDirector->Commit(entity);
        }

        {
            auto entity = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(entity, vec3(0, 100, 0));
            auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
            emitterComponent->color1 = color_green;
            emitterComponent->color2 = color_yellow;
            mDirector->AddComponent<MovementComponent>(entity, 200.0f);
            mDirector->Commit(entity);
        }

        {
            auto entity = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(entity, vec3(0, -100, 0));
            auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
            emitterComponent->color1 = color_red;
            emitterComponent->color2 = color_yellow;
            mDirector->AddComponent<MovementComponent>(entity, 200.0f);
            mDirector->Commit(entity);
        }

        {
            auto entity = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(entity, vec3(0, 0, 0));
            auto emitterComponent = mDirector->AddComponent<EmitterComponent>(entity);
            emitterComponent->color1 = color_blue;
            emitterComponent->color2 = color_white;
            mDirector->AddComponent<MovementComponent>(entity, 200.0f);
            mDirector->Commit(entity);
        }
        mDirector->Create();
    }

    inline void Update() override {
        mDirector->Update();

    }

    inline void Destroy() override {
        Free<BuddyMemory>(&mDirector);
    }
};