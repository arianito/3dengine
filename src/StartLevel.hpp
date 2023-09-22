#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"

class StartLevel : public Level {

    struct MovementComponent : public Component {
        float mSpeed;

        explicit inline MovementComponent(float speed) : mSpeed(speed) {}
    };

    struct ShooterComponent : public Component {
        float mRate{0.05f};
        float mSpeed{100.0f};
        float mLastShoot{0};

        ShooterComponent(float rate, float speed) : mRate(rate), mSpeed(speed) {}
    };

    struct ShapeComponent : public Component {
        int mType = 0;

        explicit inline ShapeComponent(int type) : mType(type) {}
    };

    struct ProjectileComponent : public Component {
        Vec3 mInitialPosition;
        float mSpeed;

        explicit inline ProjectileComponent(Vec3 pos, float speed) : mInitialPosition(pos), mSpeed(speed) {}
    };

    struct TransformComponent : public Component {
        Vec3 mPosition;
        Rot mRotation;

        explicit inline TransformComponent(Vec3 pos, Rot rot) : mPosition(pos), mRotation(rot) {}
    };

    struct ProjectileSystem : public System<ProjectileComponent, TransformComponent> {
        void Update() override {
            for (auto &compTuple: Components()) {
                TransformComponent *pTransform = std::get<TransformComponent *>(compTuple);
                ProjectileComponent *pProjectile = std::get<ProjectileComponent *>(compTuple);

                Vec3 fwd = rot_forward(pTransform->mRotation);
                fwd = vec3_mulf(fwd, pProjectile->mSpeed * gameTime->deltaTime);
                pTransform->mPosition = vec3_add(pTransform->mPosition, fwd);
                auto d = vec3_dist(pTransform->mPosition, pProjectile->mInitialPosition);
                if (d > 1000.0f) {
                    GetDirector()->DestroyEntity(pTransform->Id());
                }
            }
        }
    };

    struct MovementSystem : public System<MovementComponent, TransformComponent> {
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

    struct ShooterSystem : public System<ShooterComponent, TransformComponent> {
        void Update() override {
            auto down = input_mousepress(MOUSE_LEFT);
            for (auto &components: Components()) {
                ShooterComponent *pShooter = std::get<ShooterComponent *>(components);
                TransformComponent *pTransform = std::get<TransformComponent *>(components);
                if (down && (gameTime->time - pShooter->mLastShoot > pShooter->mRate)) {
                    auto director = GetDirector();
                    auto entityId = director->CreateEntity();
                    director->AddComponent<TransformComponent>(entityId, pTransform->mPosition, pTransform->mRotation);
                    director->AddComponent<ProjectileComponent>(entityId, pTransform->mPosition, pShooter->mSpeed);
                    director->AddComponent<ShapeComponent>(entityId, 2);
                    director->Commit(entityId);
                    pShooter->mLastShoot = gameTime->time;
                }
            }
        }
    };

    struct RenderSystem : public System<ShapeComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pShape = std::get<ShapeComponent *>(bucket);
                auto pTransform = std::get<TransformComponent *>(bucket);
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

    Director *mDirector{nullptr};

    inline void Create() override {
        mDirector = AllocNew<BuddyMemory, Director>();

        mDirector->AddSystem<MovementSystem>();
        mDirector->AddSystem<ShooterSystem>();
        mDirector->AddSystem<ProjectileSystem>();
        mDirector->AddSystem<RenderSystem>();

        auto entity = mDirector->CreateEntity();
        mDirector->AddComponent<TransformComponent>(entity, vec3_zero, rot_zero);
        mDirector->AddComponent<ShapeComponent>(entity, 0);
        mDirector->AddComponent<MovementComponent>(entity, 50.0f);
        mDirector->AddComponent<ShooterComponent>(entity, 0.001f, 200.0f);
        mDirector->Commit(entity);

        mDirector->Create();
    }

    inline void Update() override {
        mDirector->Update();
    }

    inline void Destroy() override {
        Free<BuddyMemory>(&mDirector);
    }
};