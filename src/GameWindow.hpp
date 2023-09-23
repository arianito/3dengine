#pragma once

#include <memory>


#include "engine/LevelManager.hpp"
#include "./StartLevel.hpp"
#include "./BuddyLevel.hpp"
#include "./TempLevel.hpp"

enum {
    nSamples = 200
};

struct GameWindow {
    LevelManager<> manager;
    bool debug = true;

    int idx = 0;
    float samples[nSamples];

    inline void Create() {
        manager.Add<StartLevel>();
        manager.Add<BuddyLevel>();
        manager.Add<TempLevel>();
        manager.Load<TempLevel>();
        for (float &sample: samples)
            sample = 0;
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
        if (input_keydown(KEY_TAB)) {
            debug ^= 1;
        }


        int oj = idx;
        Vec3 last = vec3((float) oj * 2.0f, samples[oj], 0);
        for (int i = idx; i < nSamples + idx; i++) {
            int j = i % nSamples;
            Vec3 nw = vec3((float) j * 2.0f, samples[j], 0);
            if (j - oj < nSamples - 1) {
                draw_line(last, nw, color_red);
            }
            last = nw;
            oj = i;
        }
        samples[idx] = 1.0f / gameTime->deltaTime;
        idx = (idx + 1) % nSamples;


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
                          alloc->stack->offset, alloc->stack->size
            );
        }


    }
};
