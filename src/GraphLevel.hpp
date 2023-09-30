#pragma once

#include <unordered_map>
#include "engine/CLevelManager.hpp"
#include "data/hash.hpp"
#include "engine/mathf.hpp"


#define use_c (0)

struct MyHash {
    inline std::size_t operator()(const Vec3 &str) const {
        return hash_type(str, 0);
    }
};

class GraphLevel : public CLevel {

#if use_c
    std::unordered_map<Vec3, float, MyHash> map{4096};
#else
    TFlatMap<Vec3, float > *map;
#endif

    void Create() override {
#if use_c
#else
        map = AllocNew<BuddyMemory, TFlatMap<Vec3, float>>(4096, false);
#endif
    }

    void Update() override {

#if use_c
            for (const auto &item: map) {
#else
            for (const auto &item: *map) {
#endif
            draw_cubef(item.first, item.second, color_red);
        }

        Ray ray = camera_screenToWorld(input->position);
        Vec3 world = ray.origin + ray.direction * 0.1f;

        if (input_mousepress(MOUSE_LEFT)) {
#if use_c

            for(int i = 0; i < 10; i++) {
                if(map.size() < 4096) {
                    world += vec3_rand(100, 100, 100);
                    world = vec3_snap(world, 10);
                    map.emplace(world, randf() * 20.0f);
                }
            }
#else
            for(int i = 0; i < 10; i++) {
                world += vec3_rand(100,100,100);
                world = vec3_snap(world, 10);
                map->Set(world, randf() * 20.0f);
            }
#endif
        }
        draw_cubef(world, 10.0f, color_yellow);

        debug_origin(vec2_zero);
#if use_c
        debug_stringf(Vec2{10, 30}, "length: %d / capacity: %d", map.size(), map.bucket_count());
#else
        debug_stringf(Vec2{10, 30}, "length: %d / capacity: %d", map->Length(), map->Capacity());
#endif

#if 1
        for (int i = -10; i <= 10; i++) {
            for (int j = -10; j <= 10; j++) {
                Vec3 v = Vec3{(float) i * 10, (float) j * 10, 0};

#if use_c
                auto it = map.find(v);
                if (it == map.end()) {
#else
                if (!map->Contains(v)) {
#endif
                    draw_cubef(v, 2, color_blue);
                }
            }
        }
#endif

    }

    void Destroy() override {
#if use_c
#else
        Free<BuddyMemory>(&map);
#endif
    }
};