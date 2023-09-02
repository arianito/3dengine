
#include "game.h"
#include "draw.h"
#include "footprint.h"
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
    footprint_create(&(MemoryDef){
        .total_memory = 512 * 1024 * 1024,
    });

    file_init("../../assets/");

    game_init();
    camera_init();
    draw_init();
    grid_init();
    debug_init();
    editor_init();
    input_init();

    while (game_loop())
    {

        draw_render();
        grid_render();
        editor_update();
        debug_update();
        input_update();
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