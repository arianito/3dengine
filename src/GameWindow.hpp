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
#include "engine/Memory.hpp"

#include "data/Array.hpp"
#include "data/SinglyLinkedList.hpp"
#include "data/DoublyLinkedList.hpp"
#include "data/Queue.hpp"
#include "data/Stack.hpp"
#include "data/Deque.hpp"
#include "data/CircularQueue.hpp"

struct GameWindow : public Object<GameWindow> {

    CircularQueue<int> arr{GlobalFreelistAllocator::instance(), 10};

    inline void Create() {
    }

    inline void Update() {

        debug_origin(vec2_zero);
        debug_string3df(
                transform_identity,
                "array: %d\nfreelist: %zu / %zu",
                arr.size(),
                freelist_capacity(alloc->freelist),
                alloc->freelist->size
        );

        for (int i = 0; i < arr.mCapacity; i++) {
            if(i == arr.mHead)
                debug_color(color_blue);
            else if(i == arr.mTail)
                debug_color(color_red);
            else
                debug_color(color_white);
            debug_stringf(
                    vec3(10.0f, 40.0f + (float) i * 24.0f, 1.0f),
                    "%d: %d",
                    i,
                    arr.mQueue[i]
            );
        }

        if (input_keydown(KEY_SPACE)) {
            arr.enqueue((int) (randf() * 10000));
        }
        if (input_keydown(KEY_M)) {
            if (arr.size() > 0)
                printf("removed! %d\n", arr.dequeue());
        }
        if (input_keydown(KEY_N)) {
            arr.clear();
        }
    }
};