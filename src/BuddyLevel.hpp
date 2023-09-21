#pragma once

#include "mathf.h"
#include "debug.h"
#include "engine/Level.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"


class BuddyLevel : public Level {
public:

    inline void Create() override {
    }

    inline void Update() override {

        debug_origin(vec2_zero);
        debug_color(color_yellow);
        float d = 8.0f;
        Vec3 pos = vec3_zero;
        debug_string3df(pos, " global(0) %d / %d", alloc->global->offset, alloc->global->size);
        pos.z -= d;
        debug_string3df(pos, " freelist(0) %d / %d", freelist_capacity(alloc->freelist), alloc->freelist->size);
        pos.z -= d;
    }
};