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
#include "data/HashTable.hpp"

struct Hello {
    float a;
    float b;
    float c;
    float d;

    inline bool operator==(const Hello &other) const {
        return a == other.a && b == other.b && c == other.c && d == other.d;
    }
};

struct GameWindow : public Object<GameWindow> {
    HashTable<unsigned int, int> hash{GlobalFreelistAllocator::instance()};

    inline void Create() {
    }

    unsigned int j = 100;

    inline void Update() {
        if (input_keypress(KEY_M)) {
            j = 100;
        }
        if (input_keypress(KEY_N) && j < 130) {
            hash.remove(j++);
        }
        if (input_keypress(KEY_SPACE) && j < 130) {
            hash.set(j++, (int) (randf() * 100));
        }
        debug_origin(vec2(0, 0));

        Vec3 pos;
        pos.z = 1;
        pos.x = 10;
        pos.y = 50;
        for (int i = 0; i < hash.mBucketCount; i++) {
            auto it = hash.mBuckets[i];
            if (it != nullptr) {
                while (it != nullptr) {
                    debug_stringf(pos, "%d :: %d", it->key, it->value);
                    it = it->next;
                    pos.x += 100.0f;
                }
                pos.y += 30.0f;
            }
            pos.x = 10.0f;
        }

        debug_origin(vec2_zero);
        debug_string3df(
                transform_identity,
                "array: %d / %d :: %d \nfreelist: %zu / %zu",
                hash.size(),
                hash.mBucketCount,
                j,
                freelist_capacity(alloc->freelist),
                alloc->freelist->size
        );
    }
};