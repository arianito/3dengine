#pragma once

#include "ctime"
#include <unordered_map>
#include "engine/CLevelManager.hpp"
#include "data/hash.hpp"
#include "data/TFlatMap.hpp"
#include "data/TFastMap.hpp"
#include "engine/mathf.hpp"
#include "engine/Trace.hpp"

#include <immintrin.h>


class GraphLevel : public CLevel {

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


    using Map = TFastMap<Vec3, Color, CustomStartLevelAllocator>;
    Map *map;
    bool deleteMode = false;

    void Create() override {
        CustomStartLevelAllocator::memory = make_freelist(32 * MEGABYTES);
        map = AllocNew<FreeListMemory, Map>();
    }

    double benchmark = 0;
    void Update() override {
        debug_origin(Vec2{0, 0});
        debug_color(color_white);

        Ray ray = camera_screenToWorld(input->position);
        Vec3 world = ray.origin + ray.direction * (camera->zoom / 5000.0f);
//        world = vec3_intersectPlane(ray.origin, ray.origin + ray.direction, vec3_zero, vec3_up);

        if (input_keydown(KEY_SPACE)) {
            map->Fit();
        }

        if (input_keydown(KEY_Z)) {
            deleteMode ^= 1;
        }

        if (input_mousepress(MOUSE_LEFT)) {
            if (deleteMode) {
                int n = 10;
                world = vec3_snap(world, 10);
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        for (int k = -n; k <= n; k++) {
                            map->Remove(world + Vec3{(float) i * 10, (float) j * 10, (float) k * 10});
                        }
                    }
                }
            } else {
                int n = 5;
                for (int i = -n; i <= n; i++) {
                    world += vec3_rand(100, 100, 100);
                    world = vec3_snap(world, 10);
                    map->Set(world, color_alpha(color_red, randf() + 0.2f));
                }
            }

        }

        if (input_keydown(KEY_M)) {
            int n = 5;
            if (deleteMode) {
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        for (int k = -n; k <= n; k++) {
                            map->Remove(Vec3{(float) i * 10, (float) j * 10, (float) k * 10});
                        }
                    }
                }
            } else {
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        for (int k = -n; k <= n; k++) {
                            map->Set(Vec3{(float) i * 10, (float) j * 10, (float) k * 10}, color_alpha(color_red, randf() + 0.1f));
                        }
                    }
                }
            }
        }

        for (const auto &node: *map) {
            draw_point(node->key, (1.22f - node->value.a) * 10.0f, node->value);
        }

        debug_stringf(Vec2{10, 20}, "map: %s -> %d / %d", deleteMode ? "delete" : "insert", map->Length(), map->Capacity());
        fill_cubef(world, 10, color_alpha(color_yellow, 0.2f));


        debug_origin(vec2(1, 1));
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d", freelist_usage(CustomStartLevelAllocator::memory), CustomStartLevelAllocator::memory->total);


    }

    void Destroy() override {
        Free<FreeListMemory>(&map);
    }
};

FreeListMemory *GraphLevel::CustomStartLevelAllocator::memory = nullptr;