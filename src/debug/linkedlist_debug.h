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
#include "debug.h"
#include "sort.h"
#include "camera.h"
#include "mem/utils.h"
#include "mem/slab.h"

SlabMemory *slab = NULL;
enum {
    npool = 1000,
};
size_t pools[npool];
int capacity = 0;
float lastHit = 0;

void *custom_alloc(size_t size) {
    return alloc_global(void, size);
}

void memorydebug_create() {
    slab = make_slab(1024, 32);
    slab->allocator = custom_alloc;
    clear(pools, sizeof(pools));
}

void memorydebug_update() {

    debug_color(color_yellow);
    debug_origin(vec2(0.5, 0.5));
    Transform t = transform_identity;

    char bf1[50];
    format_bytes((double) alloc->global->offset, bf1, 50);
    char bf2[50];
    format_bytes((double) alloc->global->size, bf2, 50);

    debug_string3df(t,
                    "hello world, this is a multiline test\nfeel free to like and subscribe\nmy fps is %d\n arena: %s / %s",
                    game->fps, bf1, bf2);

    int i = 0;
    SlabPage *it = slab->pages;
    while (it != NULL) {
        BBox b;
        b.a.x = i * 10.0f;
        b.a.y = 0;
        b.a.z = 0;

        b.b.x = i * 10.0f + 10.0f;
        b.b.y = (float) it->size;
        b.b.z = 10.0f;

        fill_bbox(b, color_gray);
        draw_bbox(b, color_black);

        unsigned int space = MEMORY_SPACE_STD(SlabPage);

        const size_t start = (size_t) it - it->padding;
        size_t cursor = it->padding + space;
        space = MEMORY_SPACE_STD(SlabObject);
        while (1) {
            size_t address = start + cursor;
            unsigned int padding = MEMORY_ALIGNMENT_STD(address, SlabObject);
            cursor += padding + slab->objectSize;
            if (cursor > it->size)
                break;

            SlabObject *obj = (SlabObject *) (address + padding - space);

            BBox b;
            b.a.x = i * 10.0f + 1.0f;
            b.a.y = (float) (cursor - slab->objectSize);
            b.a.z = 0;

            b.b.x = i * 10.0f + 9.0f;
            b.b.y = (float) (cursor);
            b.b.z = 20.0f;

            fill_bbox(b, BYTE71_GET_1(obj->next) ? color_red : color_green);
            draw_bbox(b, color_black);
        }

        it = (SlabPage *) it->next;
        i++;
    }

    if (input_keypress(KEY_SPACE) && (gameTime->time - lastHit > 0.01f)) {
        sort_quick(pools, 0, npool - 1);
        void *ptr = (void *) pools[0];
        if (ptr == NULL) {
            void *newPtr = slab_alloc(slab);
            if (newPtr != NULL) {
                pools[0] = (size_t) newPtr;
            }
        }
        lastHit = gameTime->time;
    }
    if (input_keypress(KEY_M) && (gameTime->time - lastHit > 0.01f)) {
        sort_quick(pools, 0, npool - 1);
        int a = npool - 1;
        for (int i = npool - 1; i >= 0; i--)
            if (pools[i] != 0 && i < a)
                a = i;

        sort_shuffle(pools, a, npool - 1);

        void *ptr = (void *) pools[npool - 1];
        if (ptr != NULL)
            if (slab_free(slab, &ptr)) {
                pools[npool - 1] = 0;
            }
        lastHit = gameTime->time;
    }
    if (input_keydown(KEY_N)) {
        unsigned int slabSize = slab->slabSize;
        unsigned int objSize = slab->objectSize;
        slab_destroy(&slab);
        slab = make_slab(slabSize, objSize);
    }
}