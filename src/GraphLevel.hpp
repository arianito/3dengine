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
    std::unordered_map<Vec3, float, MyHash> map;
#else
    TFlatMap<Vec3, float, SlabMemory> *map;
#endif

    void Create() override {
#if use_c
#else
        map = AllocNew<SlabMemory, TFlatMap<Vec3, float, SlabMemory>>();
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
        Vec3 world = vec3_intersectPlane(ray.origin, ray.origin + ray.direction, vec3_zero, vec3_up);
        world.z = 0;
        world = vec3_snap(world, 10);

        if (input_keydown(KEY_SPACE)) {
            p2slab_fit(alloc->slab);
        }

        if (input_mousepress(MOUSE_LEFT)) {
#if use_c
            map.emplace(world, randf() * 10.0f);
#else
            map->Set(world, randf() * 10.0f);
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
        Free<SlabMemory>(&map);
#endif
    }
};