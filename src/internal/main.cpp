
#include <stdio.h>

extern "C"
{
#include "mem/alloc.h"
#include "game.h"
#include "draw.h"
#include "mathf.h"
#include "grid.h"
#include "camera.h"
#include "editor.h"
#include "debug.h"
#include "input.h"
#include "file.h"
#include "mem/pool.h"
#include "mem/utils.h"
}

#include <memory>
#include "../GameWindow.hpp"

int main(int argc, const char *argv[])
{
	MemoryMetadata meta;
	meta.global = 32 * MEGABYTES;
	meta.stack = 1 * MEGABYTES;
	meta.freelist = 16 * MEGABYTES;
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

	{
		auto game = std::make_unique<GameWindow>();

		game->Create();

		while (game_loop())
		{
			editor_update();

			game->Update();

			input_update();
			draw_render();
			grid_render();
			debug_render();
		}
	}

	debug_terminate();
	grid_terminate();
	draw_terminate();
	camera_terminate();
	input_terminate();
	game_terminate();
	alloc_terminate();
}
