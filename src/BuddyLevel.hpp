#pragma once

#include "engine/CLevelManager.hpp"
#include "data/TAVLTree.hpp"

class BuddyLevel : public CLevel {

    TAVLTree<int, SlabMemory> *tree;

    inline void Create() override {
        tree = AllocNew<SlabMemory, TAVLTree<int, SlabMemory>>();
    }

    inline void Update() override {
        drawTree(vec3(0, 0, 100), tree->mHead);
        if (input_keypress(KEY_SPACE)) {
            tree->Add(tree->Length());
        }
        if (input_keypress(KEY_M)) {
            if (!tree->Empty())
                tree->Remove(tree->Front());
        }
        if (input_keydown(KEY_ENTER)) {
            tree->inorder(tree->mHead);
        }
    }

    inline void Destroy() override {
        Free<SlabMemory>(&tree);
    }


    inline void drawTree(Vec3 pos, TAVLTree<int, SlabMemory>::Node *node) {
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
        debug_string3df(pos, "%d\n%d", node->value, tree->balanceFactor(node));
    }

};