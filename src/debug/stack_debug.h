#pragma once


#include "input.h"
#include "mathf.h"
#include "draw.h"
#include "game.h"
#include "camera.h"
#include "mem/stack.h"
#include "mem/utils.h"

StackMemory *stack = NULL;
int j = 0;
void *pools[200];
float flyTime = -90;
float lastHit = 0;

void memorydebug_create() {
    stack = make_stack(1024);
}

void memorydebug_update() {
    fill_bbox(BBox{{-15, 0,                   -10},
                   {15,  (float) stack->size, -1}}, color_gray);

    int space = MEMORY_SPACE_STD(StackMemory);
    float end = (float) (space + stack->padding);

    draw_bbox(BBox{{-10, 0,   0},
                   {10,  end, 40}}, color_darkred);
    draw_bbox(BBox{{-10, end,                   0},
                   {10,  (float) stack->offset, 6}}, color_yellow);
    draw_bbox(BBox{{-10, end,                 0},
                   {10,  (float) stack->peak, 4}}, color_blue);

    if (stack->peak != stack->offset) {
        draw_bbox(BBox{{-10, (float) stack->peak, 0},
                       {10,  (float) stack->size, 40}}, color_darkred);
    }

    size_t start = (size_t) stack - stack->padding;
    space = MEMORY_SPACE_STD(StackMemoryNode);
    StackMemoryNode *node = (StackMemoryNode *) stack->head;

    end = (float) stack->offset;
    while (node != NULL) {

        size_t offset = BYTE71_GET_7(node->data);
        unsigned char pad = BYTE71_GET_1(node->data);

        size_t address = (size_t) node - start;

        float block = (float) (address + space - pad);
        float head = (float) (address);
        float data = (float) (address + space);

        Color c = color_red;
        if (stack->head == node)
            c = color_yellow;

        draw_bbox(BBox{{-10, block, 15},
                       {10,  head,  20}}, color_white);
        draw_bbox(BBox{{-10, head, 15},
                       {10,  data, 20}}, color_gray);
        fill_bbox(BBox{{-10, data, 0},
                       {10,  end,  25}}, c);
        draw_bbox(BBox{{-10, data, 0},
                       {10,  end,  25}}, color_black);

        if (offset == 0)
            break;
        node = (StackMemoryNode *) (start + offset);
        end = block;
    }

    if (input_keypress(KEY_SPACE) && (gameTime->time - lastHit > 0.1f)) {
        if (j < 200) {
            size_t newSize = (size_t) (randf() * 50) + 3;
            void *ptr = stack_alloc(stack, newSize, 8);
            if (ptr != NULL) {
                printf("alloc %d -> %zu \n", j, stack->offset);
                pools[j] = ptr;
                j++;
            }
        }
        lastHit = gameTime->time;
    }
    if (input_keypress(KEY_M) && (gameTime->time - lastHit > 0.1f)) {
        if (j > 0) {
            j--;
            if (stack_free(stack, &pools[j])) {
                printf("free %d -> %d \n", j, stack->offset);
            }
        }
        lastHit = gameTime->time;
    }

    if (input_keypress(KEY_ENTER)) {
        // Vec3 forward = vec3(-300, sind(flyTime) * 512 + 512, 120);
        // camera->rotation = rot(-15, 0, 0);
        // camera->position = forward;
        // camera_update();
        // flyTime += gameTime->deltaTime * 10.0f;

        Vec3 forward = vec3(0, sind(flyTime) * 512 + 400, 15);
        camera->rotation = rot(0, 90, 0);
        camera->position = forward;
        camera_update();
        flyTime += gameTime->deltaTime * 10.0f;
    }
}