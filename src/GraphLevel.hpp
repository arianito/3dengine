#pragma once

extern "C" {
#include "shader.h"
}

#include "ctime"
#include <unordered_map>
#include "engine/CLevelManager.hpp"
#include "data/hash.hpp"
#include "data/TFlatMap.hpp"
#include "data/TFastMap.hpp"
#include "engine/mathf.hpp"
#include "engine/Trace.hpp"

#include <immintrin.h>
#include "engine/CMesh.hpp"


class GraphLevel : public CLevel {
private:
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

private:
    struct Data {
        Color color;
        float size;
        float index;
    };
    using Map = TFastMap<Vec3, Data, CustomStartLevelAllocator>;
    Map *map{};
    CMeshGroup<CustomStartLevelAllocator> *group;
    bool camMode = false;
    float lastImpact = -100;

public:
    void Create() override {
        CustomStartLevelAllocator::memory = make_freelist(128 * MEGABYTES);
        map = AllocNew<CustomStartLevelAllocator, Map>();
        group = CWavefrontOBJ<CustomStartLevelAllocator>::Load("models/monkey.obj");
    }

    void Update() override {
        debug_origin(Vec2{0, 0});
        debug_color(color_white);

        Ray ray = camera_screenToWorld(input->position);
        Vec3 world = ray.origin + ray.direction * (camera->zoom / 10000.0f);

        if (input_keydown(KEY_Z)) {
            camMode ^= 1;
        }

        if (input_mousepress(MOUSE_LEFT)) {
            int n = 5;
            float f = gameTime->time;
            float r = 0.1f;
            for (int i = -n; i <= n; i++) {
                Vec3 p = world + vec3_rand(300, 300, 300);// + Vec3{0, 0, 1000.0f};
                p = vec3_snapCube(p, 20);
                Data d{};
                d.color = color_alpha(color_lerp(color_green, color_yellow, randf()), randf());
                d.size = randf() * 6.0f + 0.2f;
                d.index = f;
                map->Set(p, d);
                f += r;
            }
        }

        if (input_keydown(KEY_L)) {
            float f = gameTime->time;
            float r = 0.1f;
            for (const auto &v: group->Meshes[0]->Vertices) {
                Vec3 p = v.Position * 500;// + Vec3{0, 0, 1000.0f};
                p = vec3_snapCube(p, 10);
                Data d{};
                d.color = color_alpha(color_lerp(color_green, color_yellow, randf()), randf());
                d.size = randf() * 6.0f + 0.2f;
                d.index = f;
                map->Set(p, d);
                f += r;
            }
        }

        if (input_keydown(KEY_SPACE)) {
            lastImpact = gameTime->time;
        }
        if (input_keydown(KEY_M)) {
            map->Clear();
        }

        if (camMode) {
            camera->rotation = rot(-30, gameTime->time * 30.0f, 0);
            camera->position = rot_forward(camera->rotation) * -camera->zoom;
            camera_update();
        }

        float t = gameTime->time;//fminf(gameTime->time - lastImpact, 2) + 1.0f;

        int i = 0;
        for (const auto &node: *map) {
            const Vec3 &pos = node->key;
            const Data &data = node->value;
            Vec3 p = pos;
            float mg = vec3_mag2d(pos);

            p *= (1 / sind(mg * 0.5f + t));

            float sz = data.size + clamp(10000 / mg, 0, 10.0f);
            p.z += tand(t * i * 0.03f);

            draw_point(p, sz, data.color);
            i++;
        }

        debug_stringf(Vec2{10, 20}, "map: %d / %d", map->Length(), map->Capacity());
        debug_origin(vec2(1, 1));
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d\ntime: %.2f", freelist_usage(CustomStartLevelAllocator::memory), CustomStartLevelAllocator::memory->total, t);

    }

    void Destroy() override {
        freelist_destroy(&CustomStartLevelAllocator::memory);
    }
};

FreeListMemory *GraphLevel::CustomStartLevelAllocator::memory = nullptr;