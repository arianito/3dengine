#pragma once

#include "mathf.h"
#include "draw.h"
#include "camera.h"
#include "input.h"
#include "debug.h"
#include "data/BinarySearchTree.hpp"
#include "data/MaxHeap.hpp"

class DrawUtils {
public:

    inline static void
    drawTree(BinarySearchTree<int> &bst, Vec3 pos,
             BinarySearchTree<int>::Node *node) {
        if (node == nullptr)
            return;

        float hSpacing = 10,
                vSpacing = 20;

        auto xOffset = bst.width(node) * hSpacing * 0.5f;

        if (node->left != nullptr) {
            Vec3 newPos = {pos.x, pos.y + xOffset, pos.z - vSpacing};
            drawTree(bst, newPos, node->left);
            draw_line(pos, newPos, color_red);
        }

        if (node->right != nullptr) {
            Vec3 newPos = {pos.x, pos.y - xOffset, pos.z - vSpacing};
            drawTree(bst, newPos, node->right);
            draw_line(pos, newPos, color_red);
        }


        Ray r = camera_screenToWorld(input->position);
        float rad = 5.0f;
        Color c = color_gray;
        if (ray_hitCircle(r, pos, rad, vec3_forward, nullptr)) {
            if (input_mousedown(MOUSE_LEFT)) {
                bst.remove(node->value);
            }
            c = color_yellow;
        }
        draw_circleYZ(pos, rad, c, 16);

        debug_color(color_white);
        debug_origin(vec2(0.5f, 0.5f));
        debug_string3df(pos, "%d", node->value);
    }


    inline static int previousPowerOfTwo(int num) {
        if (num <= 0)
            return 0;

        num |= (num >> 1);
        num |= (num >> 2);
        num |= (num >> 4);
        num |= (num >> 8);
        num |= (num >> 16);

        return (num >> 1) + 1;
    }

    inline static void
    drawMaxHeap(MaxHeap<float> &heap, Vec3 pos, int i) {
        if (i < 1 || i > heap.mLength) return;

        float hSpacing = 10,
                vSpacing = 20;


        auto total = previousPowerOfTwo(heap.mLength);
        auto current = previousPowerOfTwo(i);

        int logTotal = (int)log2f((float)total);
        int logCurrent = (int)log2f((float)current);

        int xOffset = hSpacing * 0.5f * (powf(2, logTotal - logCurrent));

        if((i * 2) <= heap.mLength) {
            Vec3 newPos = {pos.x, pos.y + xOffset, pos.z - vSpacing};
            drawMaxHeap(heap, newPos, i * 2);
            draw_line(pos, newPos, color_red);
        }

        if((i * 2 + 1) <= heap.mLength) {
            Vec3 newPos = {pos.x, pos.y - xOffset, pos.z - vSpacing};
            drawMaxHeap(heap, newPos, i * 2 + 1);
            draw_line(pos, newPos, color_red);
        }


        float rad = 5.0f;
        Color c = color_gray;
        draw_circleYZ(pos, rad, c, 16);
        debug_color(color_white);
        debug_origin(vec2(0.5f, 0.5f));
        debug_string3df(pos, "%.0f", heap.mHeap[i - 1]);
    }

    inline static void
    drawMinHeap(MinHeap<float> &heap, Vec3 pos, int i) {
        if (i < 1 || i > heap.mLength) return;

        float hSpacing = 10,
                vSpacing = 20;


        auto total = previousPowerOfTwo(heap.mLength);
        auto current = previousPowerOfTwo(i);

        int logTotal = (int)log2f((float)total);
        int logCurrent = (int)log2f((float)current);

        int xOffset = hSpacing * 0.5f * (powf(2, logTotal - logCurrent));

        if((i * 2) <= heap.mLength) {
            Vec3 newPos = {pos.x, pos.y + xOffset, pos.z - vSpacing};
            drawMinHeap(heap, newPos, i * 2);
            draw_line(pos, newPos, color_red);
        }

        if((i * 2 + 1) <= heap.mLength) {
            Vec3 newPos = {pos.x, pos.y - xOffset, pos.z - vSpacing};
            drawMinHeap(heap, newPos, i * 2 + 1);
            draw_line(pos, newPos, color_red);
        }


        float rad = 5.0f;
        Color c = color_gray;
        draw_circleYZ(pos, rad, c, 16);
        debug_color(color_white);
        debug_origin(vec2(0.5f, 0.5f));
        debug_string3df(pos, "%.0f", heap.mHeap[i - 1]);
    }
};