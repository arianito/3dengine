#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


class CustomStartLevelAllocator {
public:
    static FreeListMemory *buddy;

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return freelist_alloc(buddy, size, alignment);
    }

    inline static void Free(void **ptr) {
        freelist_free(buddy, ptr);
    }
};

FreeListMemory *CustomStartLevelAllocator::buddy = nullptr;

class StartLevel : public Level {
    using TAlloc = CustomStartLevelAllocator;

    struct MovementComponent : public Component<TAlloc> {
        float mSpeed;

        explicit inline MovementComponent(float speed) : mSpeed(speed) {}
    };

    struct ShooterComponent : public Component<TAlloc> {
        float mRate{0.05f};
        float mSpeed{100.0f};
        float mLastShoot{0};

        ShooterComponent(float rate, float speed) : mRate(rate), mSpeed(speed) {}
    };

    struct ShapeComponent : public Component<TAlloc> {
        int mType = 0;

        explicit inline ShapeComponent(int type) : mType(type) {}
    };

    struct ProjectileComponent : public Component<TAlloc> {
        Vec3 mInitialPosition;
        float mSpeed;

        explicit inline ProjectileComponent(Vec3 pos, float speed) : mInitialPosition(pos), mSpeed(speed) {}
    };

    struct TransformComponent : public Component<TAlloc> {
        Vec3 mPosition;
        Rot mRotation;

        explicit inline TransformComponent(Vec3 pos, Rot rot) : mPosition(pos), mRotation(rot) {}
    };

    struct ProjectileSystem : public System<TAlloc, ProjectileComponent, TransformComponent> {
        void Update() override {
            for (auto &compTuple: Components()) {
                auto pTransform = Get<TransformComponent>(compTuple);
                auto pProjectile = Get<ProjectileComponent>(compTuple);

                Vec3 fwd = rot_forward(pTransform->mRotation);
                fwd = vec3_mulf(fwd, pProjectile->mSpeed * gameTime->deltaTime);
                pTransform->mPosition = vec3_add(pTransform->mPosition, fwd);
                auto d = vec3_dist(pTransform->mPosition, pProjectile->mInitialPosition);
                if (d > 1000.0f) {
                    mDirector->DestroyEntity(pTransform->mEntityId);
                }
            }
        }
    };

    struct MovementSystem : public System<TAlloc, MovementComponent, TransformComponent> {
        inline void Update() override {
            Ray r = camera_screenToWorld(input->position);
            Vec3 dest = vec3_intersectPlane(r.origin, vec3_add(r.origin, r.direction), vec3_zero, vec3_up);


            for (const auto &bucket: Components()) {

                auto pTransform = std::get<TransformComponent *>(bucket);
                auto pMovement = std::get<MovementComponent *>(bucket);

                Vec3 diff = vec3_norm(vec3_sub(dest, pTransform->mPosition));
                Vec3 d2 = vec3_sub(dest, vec3_mulf(diff, 40.0f));

                pTransform->mRotation = rot_lookAt(pTransform->mPosition, dest, vec3_up);
                pTransform->mPosition = vec3_moveTowards(pTransform->mPosition, d2, pMovement->mSpeed * gameTime->deltaTime);
            }
        }
    };

    struct ShooterSystem : public System<TAlloc, ShooterComponent, TransformComponent> {
        void Update() override {
            auto down = input_mousepress(MOUSE_LEFT);
            for (auto &components: Components()) {
                auto pShooter = Get<ShooterComponent>(components);
                auto pTransform = Get<TransformComponent>(components);
                if (down && (gameTime->time - pShooter->mLastShoot > pShooter->mRate)) {
                    auto entityId = mDirector->CreateEntity();
                    mDirector->AddComponent<TransformComponent>(entityId, pTransform->mPosition, pTransform->mRotation);
                    mDirector->AddComponent<ProjectileComponent>(entityId, pTransform->mPosition, pShooter->mSpeed);
                    mDirector->AddComponent<ShapeComponent>(entityId, 2);
                    mDirector->Commit(entityId);
                    pShooter->mLastShoot = gameTime->time;
                }
            }
        }
    };

    struct RenderSystem : public System<TAlloc, ShapeComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pShape = Get<ShapeComponent>(bucket);
                auto pTransform = Get<TransformComponent>(bucket);
                if (pShape->mType == 0) {
                    draw_circleXY(pTransform->mPosition, 10, color_yellow, 12);
                    draw_ray(ray_scale(ray_fromRot(pTransform->mPosition, pTransform->mRotation), 20), color_yellow);
                } else if (pShape->mType == 1) {
                    draw_cubef(pTransform->mPosition, 20, color_red);
                    draw_ray(ray_scale(ray_fromRot(pTransform->mPosition, pTransform->mRotation), 20), color_red);
                } else if (pShape->mType == 2) {
                    draw_cubef(pTransform->mPosition, 5, color_red);
                    draw_ray(ray_scale(ray_fromRot(pTransform->mPosition, pTransform->mRotation), 20), color_red);
                }
            }
        }
    };

    Director<TAlloc> *mDirector{nullptr};

    inline void Create() override {

        CustomStartLevelAllocator::buddy = make_freelist(16 * MEGABYTES);
        mDirector = AllocNew<TAlloc, Director<TAlloc>>();

        mDirector->AddSystem<MovementSystem>();
        mDirector->AddSystem<ShooterSystem>();
        mDirector->AddSystem<ProjectileSystem>();
        mDirector->AddSystem<RenderSystem>();

        auto entity = mDirector->CreateEntity();
        mDirector->AddComponent<TransformComponent>(entity, vec3_zero, rot_zero);
        mDirector->AddComponent<ShapeComponent>(entity, 0);
        mDirector->AddComponent<MovementComponent>(entity, 50.0f);
        mDirector->AddComponent<ShooterComponent>(entity, 0.1f, 500.0f);
        mDirector->Commit(entity);

        mDirector->Create();
    }

    inline void Update() override {
        mDirector->Update();
        debug_origin(vec2(1, 1));
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d", freelist_usage(CustomStartLevelAllocator::buddy), CustomStartLevelAllocator::buddy->total);
    }

    inline void Destroy() override {
        Free<TAlloc>(&mDirector);
        freelist_destroy(&CustomStartLevelAllocator::buddy);
    }
};