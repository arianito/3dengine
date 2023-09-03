
#include "game.h"
#include "draw.h"
#include "alloc.h"
#include "mathf.h"
#include "grid.h"
#include "camera.h"
#include "editor.h"
#include "debug.h"
#include "input.h"
#include "file.h"

#include "stdio.h"

int main(int argc, const char *argv[])
{
    footprint_create((MemoryDef){
        .global = 1024 * KILOBYTES,
        .stack = 512 * KILOBYTES,
        .pool64 = 512 * KILOBYTES,
        .pool128 = 512 * KILOBYTES,
        .pool256 = 512 * KILOBYTES,
    });

    file_init("../../assets/");

    game_init();
    camera_init();
    draw_init();
    grid_init();
    debug_init();
    editor_init();
    input_init();


    alloc_debug();
    while (game_loop())
    {

        input_update();
        editor_update();





        draw_render();
        grid_render();
        debug_render();
        
    }

    input_terminate();
    editor_terminate();
    camera_terminate();
    grid_terminate();
    draw_terminate();
    game_terminate();
    debug_terminate();

    footprint_terminate();
}