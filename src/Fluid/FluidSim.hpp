#pragma once

#include <thread>
#include <functional>

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/CLevelManager.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"
#include "data/hash.hpp"
#include "engine/mathf.hpp"

const Vec3i kernels[27] = {
        {-1, -1, -1},
        {-1, -1, +0},
        {-1, -1, +1},
        {-1, +0, -1},
        {-1, +0, +0},
        {-1, +0, +1},
        {-1, +1, -1},
        {-1, +1, +0},
        {-1, +1, +1},

        {+0, -1, -1},
        {+0, -1, +0},
        {+0, -1, +1},
        {+0, +0, -1},
        {0,  +0, +0},
        {+0, +0, +1},
        {+0, +1, -1},
        {+0, +1, +0},
        {+0, +1, +1},


        {+1, -1, -1},
        {+1, -1, +0},
        {+1, -1, +1},
        {+1, +0, -1},
        {+1, +0, +0},
        {+1, +0, +1},
        {+1, +1, -1},
        {+1, +1, +0},
        {+1, +1, +1},
};

const Vec3i kernels2[9] = {
        {0, -1, -1},
        {0, -1, +0},
        {0, -1, +1},
        {0, +0, -1},
        {0, +0, +0},
        {0, +0, +1},
        {0, +1, -1},
        {0, +1, +0},
        {0, +1, +1},
};

void parallel_for(int N, const std::function<void(unsigned int start, unsigned int end)> &functor) {
    constexpr int nb_threads = 16;
    unsigned int batch_size = N / nb_threads;
    unsigned int batch_remainder = N % nb_threads;
    std::thread threads[nb_threads];
    for (unsigned int i = 0; i < nb_threads; ++i) {
        unsigned start = i * batch_size;
        threads[i] = std::thread(functor, start, start + batch_size);
    }
    unsigned int start = nb_threads * batch_size;
    functor(start, start + batch_remainder);
    for (auto &t: threads) t.join();
}

struct sort_pred {
    bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) {
        return left.second < right.second;
    }
};

struct Sim {
    enum {
        n = 1600
    };
    const Vec3 halfBounds{10, 1200, 1200};
    const Vec3 gravity{0, 0, 1.0f * -9.87};
    const float damping = 0.5f;

    const float smoothing_radius{60.0f};
    float padding = 15.0f;
    const float radius = 30.0f;
    const float mass = 1.0f;
    const Vec3 checkBounds{halfBounds - radius};

    float targetDensity = 6.5f;
    float pressureMultiplier = 30.0f;

    Vec3 positions[n]{};
    Vec3 predicted_positions[n]{};
    Vec3 velocities[n]{};
    float densities[n]{};
    std::pair<unsigned int, unsigned int> spatialLookup[n]{};
    unsigned int startIndices[n]{};


    void create() {
        seedf(0);
        int m = (int) sqrtf(n);

        for (int i = 0; i < n; i++) {
            positions[i] = Vec3{0, (float) (i % m), (float) (i / m)} * padding - Vec3{0, (m * padding) / 2.0f, (m * padding) / 2.0f};
//            positions[i] = vec3_rand(0, halfBounds.y, halfBounds.z);
        }

    }

    void resolve_collisions(Vec3 &position, Vec3 &velocity) const {
        if (fabsf(position.x) > checkBounds.x) {
            position.x = (checkBounds.x) * sign(position.x);
            velocity.x *= -1.0f * damping;
        }
        if (fabsf(position.y) > checkBounds.y) {
            position.y = (checkBounds.y) * sign(position.y);
            velocity.y *= -1.0f * damping;
        }
        if (fabsf(position.z) > checkBounds.z) {
            position.z = (checkBounds.z) * sign(position.z);
            velocity.z *= -1.0f * damping;
        }
    }

    static Vec3i position_to_cell_coord(const Vec3 &a, float rad) {
        return Vec3i{
                0,
                (int) (floorf((a.y) / rad)),
                (int) (floorf((a.z) / rad)),
        };
    }

    static unsigned int hash_cell(const Vec3i &a) {
        const unsigned int p1 = 326617ul, p2 = 3292489ul, p3 = 16593127ul;
        return a.x * p1 + a.y * p2 + a.z * p3;
    }

    static unsigned int get_key_from_hash(unsigned int hash) {
        return hash % n;
    }


    void update_spatial_indices() {
        parallel_for(n, [this](unsigned int start, unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                Vec3i index = position_to_cell_coord(predicted_positions[i], smoothing_radius);
                unsigned int cell_key = get_key_from_hash(hash_cell(index));
                spatialLookup[i].first = i;
                spatialLookup[i].second = cell_key;
                startIndices[i] = UINT_MAX;
            }
        });
        std::sort(spatialLookup, spatialLookup + n, sort_pred());
        parallel_for(n, [this](unsigned int start, unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                unsigned int key = spatialLookup[i].second;
                unsigned int keyPrev = i == 0 ? UINT_MAX : spatialLookup[i - 1].second;
                if (key != keyPrev) {
                    startIndices[key] = i;
                }
            }
        });
    }

    Vec3 interact_particle(const Vec3 &pos, float rad, float strength, unsigned int pid) {
        Vec3 force{0};
        Vec3 offset = pos - positions[pid];
        float sqrDst = vec3_dot(offset, offset);
        if (sqrDst < rad * rad) {
            float dst = sqrtf(sqrDst);
            Vec3 dir = dst <= EPSILON ? vec3_zero : offset / dst;
            float center = 1 - (dst / rad);
            force += (dir * strength - velocities[pid]) * center;
        }
        force.x = 0;
        return force;
    }

    void step(float dt) {
        parallel_for(n, [this, dt](unsigned int start, unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                velocities[i] += gravity * dt;
                predicted_positions[i] = positions[i] + velocities[i] * (1 / 60.0f);
            }
        });

        update_spatial_indices();

        parallel_for(n, [this](unsigned int start, unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                densities[i] = calculate_density2(predicted_positions[i]);
            }
        });

        Ray r = camera_screenToWorld(input->position);
        Vec3 p = vec3_intersectPlane(r.origin, r.origin + r.direction, vec3_zero, vec3_forward);
        p.x = 0;
        bool down = input_mousepress(MOUSE_LEFT);

        parallel_for(n, [this, dt, down, p](unsigned int start, unsigned int end) {
            for (unsigned int i = start; i < end; i++) {
                Vec3 acceleration = calculate_force(i) / densities[i];
                velocities[i] += acceleration * dt;
                Vec3 f2 = down ? interact_particle(p, 220.0f, 1000.0f, i) : vec3_zero;
                positions[i] += -f2 * dt + velocities[i] * dt;
                resolve_collisions(positions[i], velocities[i]);
            }
        });

        debug_origin(Vec2{0.5f, 0.5f});

        for (unsigned int i = 0; i < n; i++) {
            float speed = clamp01(vec3_mag(velocities[i]) / 400.0f);
            positions[i].x = 0;
            draw_point(
                    positions[i],
                    radius,
                    color_lerp(color_green, color_red, speed)
            );
        }

        draw_bbox(BBox{-halfBounds, halfBounds}, color_gray);
    }

    float calculate_density2(const Vec3 &pos) {
        float density{0};
        float sqrRad = smoothing_radius * smoothing_radius;
        Vec3i index = position_to_cell_coord(pos, smoothing_radius);
        for (auto kernel: kernels2) {
            unsigned int key = get_key_from_hash(hash_cell(index + kernel));
            unsigned int startIndex = startIndices[key];
            for (unsigned int i = startIndex; i < n; i++) {
                const auto &spatial = spatialLookup[i];
                if (spatial.second != key) break;

                unsigned int pid = spatial.first;
                float sqrDst = vec3_sqrMag((predicted_positions[pid] - pos));


                if (sqrDst <= sqrRad) {
                    float influence = smoothing_kernel(sqrtf(sqrDst), smoothing_radius);
                    density += mass * influence;
                }
            }
        }
        return density;
    }

private:
    [[nodiscard]]
    float density_to_pressure(float density) const {
        float err = density - targetDensity;
        return err * pressureMultiplier;
    }

    static float smoothing_kernel(float dst, float rad) {
        if (dst >= rad) return 0;
        float volume = PI * powf(rad, 4) / 6;
        float offset = rad - dst;
        return (offset * offset) / volume;
    }

    static float smoothing_kernel_d(float dst, float rad) {
        if (dst >= rad) return 0;
        float scale = 12.0f / (powf(rad, 4) * PI);
        return scale * (dst - rad);
    }

    float calculate_density(const Vec3 &pos) {
        float density{0};
        for (unsigned int i = 0; i < n; i++) {
            float dst = vec3_dist(positions[i], pos);
            float influence = smoothing_kernel(dst, smoothing_radius);
            density += mass * influence;
        }
        return density;
    }


    float shared_pressure(float density1, float density2) {
        float p1 = density_to_pressure(density1);
        float p2 = density_to_pressure(density2);
        return (p1 + p2) / 2.0f;
    }

    Vec3 calculate_force(unsigned int pid) {
        Vec3 grad{0, 0, 0};
        for (unsigned int i = 0; i < n; i++) {
            if (i == pid) continue;
            Vec3 offset = predicted_positions[pid] - predicted_positions[i];
            float dst = vec3_mag(offset);
            Vec3 dir = (dst == 0) ? vec3_rand(0, 1, 1) : (offset / dst);
            float slope = smoothing_kernel_d(dst, smoothing_radius);
            float density = densities[i];
            float sharedPressure = shared_pressure(density, densities[pid]);
            grad += sharedPressure * dir * slope * mass / densities[i];
        }
        return grad;
    }

};

class FluidSim : public CLevel {
    Sim sim;

    void Create() override {
        sim.create();
    }

    void Update() override {
        sim.step(gameTime->deltaTime * 2);
    }

    void Destroy() override {
    }
};