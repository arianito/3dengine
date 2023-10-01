#pragma once

#include <unordered_map>
#include "engine/CLevelManager.hpp"
#include "data/hash.hpp"
#include "data/TFlatMap.hpp"
#include "data/TFastMap.hpp"
#include "engine/mathf.hpp"
#include "engine/Trace.hpp"

#include <immintrin.h>

class GraphLevel : public CLevel {
    TFastMap<Vec3, Color> map;
    bool deleteMode = false;

    void Create() override {

    }

    void Update() override {
        debug_origin(Vec2{0, 0});
        debug_color(color_white);

        Ray ray = camera_screenToWorld(input->position);
        Vec3 world = ray.origin + ray.direction * 0.1f;
        world = vec3_intersectPlane(ray.origin, ray.origin + ray.direction, vec3_zero, vec3_up);
        world.z = 0;

        if (input_keydown(KEY_Z)) {
            deleteMode ^= 1;
        }

        if (input_mousepress(MOUSE_LEFT)) {
            world = vec3_snap(world, 10);
            if (deleteMode) {
                map.Remove(world);
            } else {
                map.Set(world, color_alpha(color_red, randf() + 0.2f));
            }

        }

        if (input_keydown(KEY_M)) {
            int n = 50;
            if (deleteMode) {
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        map.Remove(Vec3{(float) i * 10, (float) j * 10, 0});
                    }
                }
            } else {
                for (int i = -n; i <= n; i++) {
                    for (int j = -n; j <= n; j++) {
                        map.Set(Vec3{(float) i * 10, (float) j * 10, 0}, color_alpha(color_red, randf() + 0.2f));
                    }
                }
            }
        }

        if (input_keydown(KEY_ENTER)) {

        }
        for (const auto &node: map) {
            draw_point(node->key, 5, node->value);
        }

        debug_stringf(Vec2{10, 20}, "map: %s -> %d / %d", deleteMode ? "delete" : "insert", map.Length(), map.Capacity());
        fill_cubef(world, 10, color_alpha(color_yellow, 0.2f));
    }

    void Destroy() override {
    }
};