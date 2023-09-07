
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
#include "debug/arena_debug.h"
}

#include "engine/Component.hpp"

class Box : public Entity
{
	using Entity::Entity;

	void Create() override
	{
		printf("create box\n");
	}

	void Update() override
	{
		printf("update box\n");
	}

	void Destroy() override
	{
		printf("destroy box\n");
	}
};

int main(int argc, const char *argv[])
{
	MemoryMetadata meta;
	meta.components = 100 * KILOBYTES;
	meta.stack = 100 * KILOBYTES;
	meta.global = 2 * MEGABYTES;

	alloc_create(meta);

	file_init("../../assets/");
	game_init();
	input_init();
	camera_init();
	draw_init();
	grid_init();
	debug_init();
	editor_init();

	auto director = MakeDirector();
	director->AddEntity<Box>();
	director->Create();

	alloc_debug();

	memorydebug_create();

	while (game_loop())
	{
		editor_update();

		memorydebug_update();

		director->Update();

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
