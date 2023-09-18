#pragma once


#include "input.h"
#include "mathf.h"
#include "draw.h"
#include "game.h"
#include "camera.h"
#include "mem/arena.h"
#include "mem/utils.h"

ArenaMemory *arena = NULL;
float flyTime = -90;
float lastHit = 0;

void memorydebug_create() {
    arena = make_arena(KILOBYTES);
}

void memorydebug_update() {

    fill_bbox(BBox{{-15, 0,                   -10},
                   {15,  (float) arena->size, -1}}, color_gray);

    int space = MEMORY_SPACE_STD(ArenaMemory);
    float end = (float) (space + arena->padding);

    draw_bbox(BBox{{-10, 0,   0},
                   {10,  end, 40}}, color_darkred);
    draw_bbox(BBox{{-10, end,                   0},
                   {10,  (float) arena->offset, 6}}, color_yellow);

    fill_bbox(BBox{{-10, end,                   0},
                   {10,  (float) arena->offset, 25.0f}}, color_red);
    draw_bbox(BBox{{-10, end,                   0},
                   {10,  (float) arena->offset, 25.0f}}, color_black);

    if (space + arena->padding != arena->offset) {
        draw_bbox(BBox{{-10, (float) arena->offset, 0},
                       {10,  (float) arena->size,   40}}, color_darkred);
    }

    if (input_keydown(KEY_SPACE)) {
        size_t newSize = (size_t) (randf() * 20) + 3;
        void *ptr = arena_alloc(arena, newSize, 8);
        if (ptr != NULL) {
            printf("alloc %zu \n", arena->offset);
        }
    }
}