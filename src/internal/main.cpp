
#include <memory>

extern "C" {
#include "mem/alloc.h"
#include "game.h"
#include "draw.h"
#include "grid.h"
#include "camera.h"
#include "editor.h"
#include "debug.h"
#include "input.h"
#include "file.h"
}

#include "../GameWindow.hpp"

int main(int argc, const char *argv[]) {
    MemoryMetadata meta;
    meta.boot = 64 * MEGABYTES;

    meta.global = 32 * MEGABYTES;
    meta.freelist = 8 * MEGABYTES;
    meta.buddy = 8 * MEGABYTES;
    meta.stack = 1 * MEGABYTES;
    meta.string = 1 * MEGABYTES;

    alloc_create(meta);


    file_init("../assets/");
    game_init();
    input_init();
    camera_init();
    draw_init();
    grid_init();
    debug_init();
    editor_init();

    {
        GameWindow window;

        window.Create();

        while (game_loop()) {
            input_update();

            grid_render();
            editor_update();
            draw_render();
            debug_render();

            window.Update();
        }
    }

    debug_terminate();
    grid_terminate();
    draw_terminate();
    input_terminate();
    game_terminate();
    alloc_terminate();
}
