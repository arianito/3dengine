
#include <stdio.h>


extern "C"
{
#include "memory/alloc.h"
#include "game.h"
#include "draw.h"
#include "mathf.h"
#include "grid.h"
#include "camera.h"
#include "editor.h"
#include "debug.h"
#include "input.h"
#include "file.h"
}

#include "Engine/Component.hpp"

int main(int argc, const char *argv[])
{
    MemoryMetadata meta;
    meta.components = 128 * KILOBYTES;
    meta.stack = 128 * KILOBYTES;
    meta.global = 1024 * KILOBYTES;

    alloc_create(meta);

    file_init("../../assets/");

    game_init();
    input_init();
    camera_init();
    draw_init();
    grid_init();
    debug_init();
    editor_init();

    alloc_debug();
    while (game_loop())
    {

        input_update();
        editor_update();

        draw_render();
        grid_render();
        debug_render();
    }

    debug_terminate();
    grid_terminate();
    draw_terminate();
    camera_terminate();
    input_terminate();
    game_terminate();

    alloc_terminate();
}