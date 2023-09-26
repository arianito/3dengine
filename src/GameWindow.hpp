#pragma once

#include <memory>


#include "engine/CLevelManager.hpp"

#include "./StartLevel.hpp"
#include "./BuddyLevel.hpp"
#include "./TempLevel.hpp"
#include "./MeshLevel.hpp"

enum {
    nSamples = 200
};

struct GameWindow {
    CLevelManager<> manager;
    bool debug = true;

    inline void Create() {
        manager.Add<StartLevel>();
        manager.Add<BuddyLevel>();
        manager.Add<TempLevel>();
        manager.Add<MeshLevel>();

        manager.Load<MeshLevel>();
    }

    inline void Update() {
        manager.Update();

        if (input_keydown(KEY_0)) {
            manager.Load<StartLevel>();
        }
        if (input_keydown(KEY_9)) {
            manager.Load<TempLevel>();
        }
        if (input_keydown(KEY_8)) {
            manager.Load<BuddyLevel>();
        }
        if (input_keydown(KEY_7)) {
            manager.Load<MeshLevel>();
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
                               "string %d / %d\n"
                               "buddy %d / %d\n"
                               "stack %d / %d",
                          alloc->boot->usage, alloc->boot->total,
                          alloc->global->usage, alloc->global->total,
                          freelist_usage(alloc->freelist), alloc->freelist->total,
                          freelist_usage(alloc->string), alloc->string->total,
                          alloc->buddy->usage, alloc->metadata.buddy,
                          alloc->stack->usage, alloc->stack->total
            );
        }


    }
};
