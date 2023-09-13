#pragma once
/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/

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