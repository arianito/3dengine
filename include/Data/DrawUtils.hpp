#pragma once

#include "mathf.h"
#include "draw.h"
#include "camera.h"
#include "input.h"
#include "debug.h"
#include "data/BinarySearchTree.hpp"

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
        if (ray_hitCircle(r, pos, rad, vec3_forward, NULL)) {
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

};