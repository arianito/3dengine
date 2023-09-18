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
#include "data/BinarySearch.hpp"
#include "data/BinarySearchTree.hpp"
#include "data/FixedStack.hpp"
#include "data/MaxHeap.hpp"
#include "data/MinHeap.hpp"
#include "data/DrawUtils.hpp"

struct GameWindow : public Object<GameWindow> {
    MinHeap<float> heap{GlobalFreelistAllocator::instance()};

    inline void Create() {
    }

    inline void Update() {
        debug_rotation(rot_inv(camera->rotation));

        if(input_keydown(KEY_SPACE)) {
            for(int i = 0;i < 5; i++)
                heap.push(floorf(randf() * 99));
        }

        if (input_keydown(KEY_M) && !heap.empty()) {
            heap.pop();
        }

        debug_origin(vec2_zero);
        debug_stringf(vec2(10, 40), "validate: %d", heap.validate(1));

        DrawUtils::drawMinHeap(heap, vec3(0, 0, 50), 1);
    }
};
