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

#include <ctime>

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
#include "data/ProbeHashTable.hpp"
#include "data/Tree.hpp"

struct GameWindow : public Object<GameWindow> {

    inline void Create() {
    }

    inline void Update() {

        debug_origin(vec2_zero);
        debug_string3df(
                transform_identity,
                "freelist: %zu / %zu",
                freelist_capacity(alloc->freelist),
                alloc->freelist->size
        );
    }
};