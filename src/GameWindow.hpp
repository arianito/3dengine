#pragma once

#include <memory>


#include "engine/CLevelManager.hpp"

#include "./GraphLevel.hpp"
#include "./MeshLevel.hpp"
#include "./TestLevel.hpp"
#include "./TempLevel.hpp"

enum {
    nSamples = 200
};

struct GameWindow {
    CLevelManager<> manager;
    bool debug = true;

    inline void Create() {
        manager.Add<GraphLevel>();
        manager.Add<TestLevel>();
        manager.Add<MeshLevel>();
        manager.Add<TempLevel>();

        manager.Load<GraphLevel>();
    }

    inline void Update() {
        manager.Update();

        if (input_keydown(KEY_0)) {
            manager.Load<GraphLevel>();
        }
        if (input_keydown(KEY_9)) {
            manager.Load<TestLevel>();
        }
        if (input_keydown(KEY_8)) {
            manager.Load<MeshLevel>();
        }
        if (input_keydown(KEY_7)) {
            manager.Load<TempLevel>();
        }



        if (input_keydown(KEY_TAB)) {
            debug ^= 1;
        }
        if (debug) {
            debug_origin(vec2(0, 1));
            debug_color(color_yellow);
            Vec2 pos = vec2(10, game->height - 10);
            debug_stringf(pos, "boot %d / %d\n"
                               "global %d / %d\n"
                               "freelist %d / %d\n"
                               "slab %d / %d\n"
                               "string %d / %d\n"
                               "buddy %d / %d\n"
                               "stack %d / %d",
                          alloc->boot->usage, alloc->boot->total,
                          alloc->global->usage, alloc->global->total,
                          freelist_usage(alloc->freelist), alloc->freelist->total,
                          alloc->slab->usage, alloc->slab->total,
                          freelist_usage(alloc->string), alloc->string->total,
                          alloc->buddy->usage, alloc->metadata.buddy,
                          alloc->stack->usage, alloc->stack->total
            );
        }
    }
};
