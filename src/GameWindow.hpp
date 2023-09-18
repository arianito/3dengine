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

#include "engine/Object.hpp"
#include "data/Heap.hpp"
#include "data/ProbeHashTable.hpp"
#include "data/DrawUtils.hpp"

struct GameWindow : public Object<GameWindow> {
    Heap<float> h{GlobalFreelistAllocator::instance(), MIN_HEAP};

    inline void Create() {
    }

    inline void Update() {
        debug_rotation(rot_inv(camera->rotation));

        if (input_keydown(KEY_B)) {
            const float buff[] = {2, 4, 5, 6, 3, 42, 8, 124, 22, 566};
            h.heapify(buff, 10);
        }
        if (input_keydown(KEY_SPACE)) {
            h.push(randf() * 100.0f);
        }
        if (input_keydown(KEY_M)) {
            h.pop();
        }

        DrawUtils::drawHeap(h, vec3(0, 0, 50), 1);

        debug_origin(vec2(0,0));
        debug_stringf(vec2(10, 50), "%d / %d",h.mLength, h.mCapacity);
    }
};
