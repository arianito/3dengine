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

struct GameWindow : public Object<GameWindow> {
    using Node = BinarySearchTree<int>::Node;

    BinarySearchTree<int> bst{GlobalFreelistAllocator::instance()};
    CircularQueue<Node *> queue{GlobalFreelistAllocator::instance(), 100};

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


    inline void
    drawTree(Vec3 pos, BinarySearchTree<int>::Node *node) {
        if (node == nullptr)
            return;

        float hSpacing = 10,
                vSpacing = 20;

        auto xOffset = bst.width(node) * hSpacing * 0.5f;

        if (node->left != nullptr) {
            Vec3 newPos = {pos.x, pos.y + xOffset, pos.z - vSpacing};
            drawTree(newPos, node->left);
            draw_line(pos, newPos, color_red);
        }

        if (node->right != nullptr) {
            Vec3 newPos = {pos.x, pos.y - xOffset, pos.z - vSpacing};
            drawTree(newPos, node->right);
            draw_line(pos, newPos, color_red);
        }

        draw_circleYZ(pos, color_yellow, 5.0f, 16);

        debug_color(color_white);
        debug_origin(vec2(0.5f, 0.5f));
        debug_string3df(pos, "%d", node->value);
    }


    inline void Update() {

        if (input_keydown(KEY_SPACE)) {
            bst.remove(17);
        }
        debug_rotation(rot_inv(camera->rotation));

        drawTree(vec3(0, 0, 100), bst.mRoot);
        debug_origin(vec2(0,0));

        debug_stringf(vec2(10, 20), "%d", bst.validate());
    }
};
