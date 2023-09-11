
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
#include "memory/pool.h"
#include "memory/utils.h"
#include "debug/linkedlist_debug.h"
}

#include "engine/Manager.hpp"

class Box : public Component
{
};

class Sphere : public Component
{
};

int main(int argc, const char *argv[])
{
	MemoryMetadata meta;
	meta.global = 32 * MEGABYTES;
	meta.stack = 1 * MEGABYTES;

	alloc_create(meta);

	file_init("../../assets/");
	game_init();
	input_init();
	camera_init();
	draw_init();
	grid_init();
	debug_init();
	editor_init();

	MakeDirector();
	auto director = MakeDirector();

	size_t e = director->CreateEntity();

	director->Create();

	alloc_debug();

	memorydebug_create();

	while (game_loop())
	{
		editor_update();
		// loop

		director->Update();
		memorydebug_update();

		input_update();
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
