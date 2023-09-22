#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


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
        for (auto &compTuple: GetComponents()) {
            TransformComponent *pTransform = std::get<TransformComponent *>(compTuple);
            ProjectileComponent *pProjectile = std::get<ProjectileComponent *>(compTuple);

            Vec3 fwd = rot_forward(pTransform->mRotation);
            fwd = vec3_mulf(fwd, pProjectile->mSpeed * gameTime->deltaTime);
            pTransform->mPosition = vec3_add(pTransform->mPosition, fwd);
            auto d = vec3_dist(pTransform->mPosition, pProjectile->mInitialPosition);
            if (d > 1000.0f) {
                GetDirector()->DestroyEntity(pTransform->GetEntityId());
            }
        }
    }
};

struct MovementSystem : public System<MovementComponent, TransformComponent> {
    inline void Update() override {
        auto vertical = input_axis(AXIS_VERTICAL);
        auto horizontal = input_axis(AXIS_HORIZONTAL);
        for (const auto &bucket: GetComponents()) {
            auto pTransform = std::get<TransformComponent *>(bucket);
            auto pMovement = std::get<MovementComponent *>(bucket);
            pTransform->mRotation.yaw += horizontal * pMovement->mSpeed * gameTime->deltaTime;
            Vec3 fwd = vec3_mulf(rot_forward(pTransform->mRotation), vertical * pMovement->mSpeed * gameTime->deltaTime);
            pTransform->mPosition = vec3_add(pTransform->mPosition, fwd);
        }
    }
};

struct ShooterSystem : public System<ShooterComponent, TransformComponent> {
    void Update() override {
        auto down = input_keypress(KEY_SPACE);
        for (auto &components: GetComponents()) {
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
        for (const auto &bucket: GetComponents()) {
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

class StartLevel : public Level {
public:
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
        mDirector->AddComponent<MovementComponent>(entity, 200.0f);
        mDirector->AddComponent<ShooterComponent>(entity, 0.1f, 600.0f);
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