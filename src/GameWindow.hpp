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
#include "data/DrawUtils.hpp"

struct GameWindow : public Object<GameWindow> {
    BinarySearchTree<int> bst{GlobalFreelistAllocator::instance()};

    inline void Create() {
        bst.insert(12);
        bst.insert(5);
        bst.insert(15);
        bst.insert(3);
        bst.insert(7);
        bst.insert(13);
        bst.insert(17);
        bst.insert(1);
        bst.insert(4);
        bst.insert(6);
        bst.insert(9);
        bst.insert(16);
        bst.insert(19);
        bst.insert(8);
        bst.insert(14);
        bst.insert(25);
    }

    inline void Update() {
        debug_rotation(rot_inv(camera->rotation));

        DrawUtils::drawTree(bst, vec3(0, 0, 40), bst.mRoot);

        debug_origin(vec2(0, 0));
        debug_stringf(vec2(10, 20), "%d", bst.validate());
    }
};
