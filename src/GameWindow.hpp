#pragma once

#include <memory>

extern "C"
{
#include "draw.h"
#include "game.h"
#include "mathf.h"
#include "mem/freelist.h"
#include "mem/utils.h"
}

#include "data/Array.hpp"
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

struct GameWindow : public Object<GameWindow> {

    Array<int> arr{GlobalFreelistAllocator::instance()};

    inline void Create() {
        arr.push(23);
    }

    inline void Update() {

        debug_string3df(transform_identity, "array: %d / %d\nfreelist: %zu / %zu", arr.size(), arr.capacity(),
                        freelist_capacity(alloc->freelist), alloc->freelist->size);

        if(input_keydown(KEY_SPACE)) {
            arr.push((int)(randf() * 10000));
        }
        if(input_keydown(KEY_N)) {
            arr.clear();
        }
    }
};