
extern "C"
{
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

#include <memory>
#include "../GameWindow.hpp"

int main(int argc, const char *argv[]) {
    MemoryMetadata meta;
    meta.global = 64 * MEGABYTES;
    meta.stack = 1 * MEGABYTES;
    meta.freelist = 16 * MEGABYTES;
    alloc_create(meta);

    file_init("../assets/");
    game_init();
    input_init();
    camera_init();
    draw_init();
    grid_init();
    debug_init();
    editor_init();
    alloc_debug();

    {
        auto window = std::make_unique<GameWindow>();

        window->Create();

        while (game_loop()) {
            editor_update();

            window->Update();

            input_update();
            draw_render();
            grid_render();
            debug_render();
        }
    }

    alloc_debug();
    debug_terminate();
    grid_terminate();
    draw_terminate();
    input_terminate();
    game_terminate();
    alloc_terminate();
}
