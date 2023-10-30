#pragma once

#include "engine/CLevelManager.hpp"
#include "data/TAVLTree.hpp"
#include "data/TBinaryTree.hpp"

class BuddyLevel : public CLevel {

    using Tree = TBinaryTree<int, SlabMemory>;
//    TAVLTree<int, SlabMemory> *tree;
    Tree *tree;

    inline void Create() override {
        tree = AllocNew<SlabMemory, TBinaryTree<int, SlabMemory>>();
    }

    inline void Update() override {
        drawTree(vec3(0, 0, 100), tree->mRoot);
        if (input_keydown(KEY_SPACE)) {
            tree->Add((int)(randf() * 1000));
        }
        if (input_keydown(KEY_M)) {
            if (!tree->Empty()) tree->Remove(tree->Front());
        }
    }

    inline void Destroy() override {
        Free<SlabMemory>(&tree);
    }


    inline void drawTree(Vec3 pos, Tree::Node *node) {
        if (node == nullptr)
            return;

        float vSpacing = 20,
                hSpacing = 20.0f;

        auto xOffset = tree->size(node) * hSpacing * 0.5f;

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


        Ray r = camera_screenToWorld(input->position);
        float rad = 10.0f;
        Color c = color_gray;
        if (ray_hitSphere(r, sphere(pos, rad), nullptr)) {
            if (input_mousedown(MOUSE_LEFT)) {
                tree->Remove(node->value);
            }
            c = color_yellow;
        }
        debug_rotation(rot_inv(camera->rotation));
        draw_circleYZ(pos, rad, c, 16);

        debug_color(color_white);
        debug_origin(vec2(0.5f, 0.5f));
        debug_string3df(pos, "%d", node->value);
    }

};