#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/CLevelManager.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


class CustomStartLevelAllocator {
public:
    static FreeListMemory *memory;

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return freelist_alloc(memory, size, alignment);
    }

    inline static void Free(void **ptr) {
        freelist_free(memory, ptr);
    }
};

FreeListMemory *CustomStartLevelAllocator::memory = nullptr;

class StartLevel : public CLevel {
    using TAlloc = CustomStartLevelAllocator;

    struct TransformComponent : public CComponent<TAlloc> {
        Vec3 position;
        Rot rotation;

        explicit inline TransformComponent(Vec3 pos, Rot rot) : position(pos), rotation(rot) {}
    };

    struct CanonComponent : public CComponent<TAlloc> {
        float fov{45};
        float speed{200};
        float shootRate{0.05f};
        float bulletSpeed{100.0f};
        float lastSuccessfulShoot{0};

        explicit inline CanonComponent(float fov, float speed, float rate, float bulletSpeed) : fov(fov), speed(speed), shootRate(rate),
                                                                                                bulletSpeed(bulletSpeed) {}
    };

    struct MovementComponent : public CComponent<TAlloc> {
        float speed;

        explicit inline MovementComponent(float speed) : speed(speed) {}
    };

    struct ColliderComponent : public CComponent<TAlloc> {
        float radius{10.0f};

        explicit inline ColliderComponent(float radius) : radius(radius) {}
    };

    struct PlayerComponent : public CComponent<TAlloc> {
        float shootRate{0.05f};
        float bulletSpeed{100.0f};
        float lastSuccessfulShoot{0};

        PlayerComponent(float rate, float bulletSpeed) : shootRate(rate), bulletSpeed(bulletSpeed) {}
    };

    struct ShapeComponent : public CComponent<TAlloc> {
        int shapeType = 0;

        explicit inline ShapeComponent(int type) : shapeType(type) {}
    };

    struct ProjectileComponent : public CComponent<TAlloc> {
        Vec3 initialPosition;
        float speed;

        explicit inline ProjectileComponent(Vec3 pos, float speed) : initialPosition(pos), speed(speed) {}
    };


    struct ProjectileSystem : public CSystem<TAlloc, ProjectileComponent, TransformComponent> {
        void Update() override {
            for (auto &compTuple: Components()) {
                auto bulletTransform = Get<TransformComponent>(compTuple);
                auto projectile = Get<ProjectileComponent>(compTuple);
                bulletTransform->position += rot_forward(bulletTransform->rotation) * (projectile->speed * gameTime->deltaTime);
                if (vec3_sqrMag(bulletTransform->position - projectile->initialPosition) > 1000000.0f)
                    mDirector->DestroyEntity(bulletTransform->EntityId());
            }
        }
    };

    struct MovementSystem : public CSystem<TAlloc, MovementComponent, TransformComponent> {
        inline void Update() override {
            Ray inputRay = camera_screenToWorld(input->position);
            Vec3 mousePos = vec3_intersectPlane(inputRay.origin, vec3_add(inputRay.origin, inputRay.direction), vec3_zero, vec3_up);

            float yAxis = input_axis(AXIS_HORIZONTAL);
            float xAxis = input_axis(AXIS_VERTICAL);

            for (const auto &bucket: Components()) {

                auto playerTransform = std::get<TransformComponent *>(bucket);
                auto movement = std::get<MovementComponent *>(bucket);

                Vec3 move{xAxis, yAxis, 0};
                Rot direction{0, camera->rotation.yaw, 0};
                playerTransform->position += rot_rotate(direction, move) * gameTime->deltaTime * movement->speed;
                playerTransform->rotation = rot_lookAt(playerTransform->position, mousePos, vec3_up);
            }
        }
    };

    struct ShooterSystem : public CSystem<TAlloc, PlayerComponent, TransformComponent> {
        void Update() override {
            auto down = input_mousepress(MOUSE_LEFT);
            for (auto &components: Components()) {
                auto pShooter = Get<PlayerComponent>(components);
                auto pTransform = Get<TransformComponent>(components);
                if (down && (gameTime->time - pShooter->lastSuccessfulShoot > pShooter->shootRate)) {
                    auto entityId = mDirector->CreateEntity();
                    mDirector->AddComponent<TransformComponent>(entityId, pTransform->position, pTransform->rotation);
                    mDirector->AddComponent<ProjectileComponent>(entityId, pTransform->position, pShooter->bulletSpeed);
                    mDirector->AddComponent<ShapeComponent>(entityId, 1);
                    mDirector->Commit(entityId);
                    pShooter->lastSuccessfulShoot = gameTime->time;
                }
            }
        }
    };

    struct RenderSystem : public CSystem<TAlloc, ShapeComponent, TransformComponent> {
        inline void Update() override {
            for (const auto &bucket: Components()) {
                auto pShape = Get<ShapeComponent>(bucket);
                auto pTransform = Get<TransformComponent>(bucket);
                if (pShape->shapeType == 0) {
                    draw_circleXY(pTransform->position, 10, color_yellow, 12);
                    draw_ray(ray_scale(ray_fromRot(pTransform->position, pTransform->rotation), 20), color_yellow);
                } else if (pShape->shapeType == 1) {
                    draw_cubef(pTransform->position, 5, color_red);
                    draw_ray(ray_scale(ray_fromRot(pTransform->position, pTransform->rotation), 20), color_red);
                } else if (pShape->shapeType == 2) {
                    draw_circleXY(pTransform->position, 10, color_blue, 12);
                    draw_ray(ray_scale(ray_fromRot(pTransform->position, pTransform->rotation), 20), color_blue);
                }
            }
        }
    };

    struct CanonSystem : public CSystem<TAlloc, CanonComponent, TransformComponent> {
        TransformComponent *playerTransform{};
        inline void Update() override {
            if (playerTransform == nullptr) {
                auto players = mDirector->Query<PlayerComponent>();
                if (players != nullptr) {
                    for (const auto &p: *players) {
                        playerTransform = p->value->GetComponent<TransformComponent>();
                        break;
                    }
                }
            }
            float n = (float)Components().Length();
            float p = 360.0f / n;
            float r = 0;
            for (const auto &bucket: Components()) {
                auto canonTransform = Get<TransformComponent>(bucket);
                auto canon = Get<CanonComponent>(bucket);
                if (playerTransform != nullptr) {
                    Vec3 targetPosition = Vec3{cosd(r), sind(r), 0} * 50 + playerTransform->position - (~(playerTransform->position - canonTransform->position) * 80.0f);

                    canonTransform->rotation = rot_lerp(canonTransform->rotation,
                                                        rot_lookAt(canonTransform->position, playerTransform->position, vec3_up),
                                                        10.0f * gameTime->deltaTime);

                    canonTransform->position = vec3_moveTowards(canonTransform->position,
                                                                targetPosition,
                                                                canon->speed * gameTime->deltaTime);


                    bool canShoot = vec3_sqrMag(canonTransform->position - playerTransform->position) < 40000;
                    if (canShoot && (gameTime->time - canon->lastSuccessfulShoot > canon->shootRate)) {
                        auto entityId = mDirector->CreateEntity();
                        mDirector->AddComponent<TransformComponent>(
                                entityId,
                                canonTransform->position,
                                canonTransform->rotation
                        );
                        mDirector->AddComponent<ProjectileComponent>(entityId, canonTransform->position, canon->bulletSpeed);
                        mDirector->AddComponent<ShapeComponent>(entityId, 1);
                        mDirector->Commit(entityId);
                        canon->lastSuccessfulShoot = gameTime->time;
                    }
                }
                r += p;
            }
        }
    };

    struct CollisionSystem : public CSystem<TAlloc, ColliderComponent, TransformComponent> {
        inline void Update() override {
            for(const auto& bucket: Components()) {

            }

        }
    };

    CDirector<TAlloc> *mDirector{nullptr};

    inline void Create() override {

        CustomStartLevelAllocator::memory = make_freelist(16 * MEGABYTES);
        mDirector = AllocNew<TAlloc, CDirector<TAlloc>>();
        mDirector->AddSystem<MovementSystem>();
        mDirector->AddSystem<ShooterSystem>();
        mDirector->AddSystem<ProjectileSystem>();
        mDirector->AddSystem<RenderSystem>();
        mDirector->AddSystem<CanonSystem>();


        auto player = mDirector->CreateEntity();
        mDirector->AddComponent<TransformComponent>(player, vec3_zero, rot_zero);
        mDirector->AddComponent<ShapeComponent>(player, 0);
        mDirector->AddComponent<MovementComponent>(player, 250.0f);
        mDirector->AddComponent<PlayerComponent>(player, 0.1f, 1000.0f);
        mDirector->Commit(player);
        int n = 5;
        for (int i = -n; i <= n; i++) {
            auto canon = mDirector->CreateEntity();
            mDirector->AddComponent<TransformComponent>(canon, Vec3{(float) i * 20.0f, 100, 0}, rot_zero);
            mDirector->AddComponent<ShapeComponent>(canon, 2);
            mDirector->AddComponent<CanonComponent>(canon, 45, 100, 0.2f, 500.0f);
            mDirector->Commit(canon);
        }

        mDirector->Create();
    }

    inline void Update() override {
        mDirector->Update();
        debug_origin(vec2(1, 1));
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d", freelist_usage(CustomStartLevelAllocator::memory), CustomStartLevelAllocator::memory->total);

        if (input_keydown(KEY_SPACE)) {
            mDirector->Fit();
        }
    }

    inline void Destroy() override {
        Free<TAlloc>(&mDirector);
        freelist_destroy(&CustomStartLevelAllocator::memory);
    }
};