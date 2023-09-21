cmake_minimum_required(VERSION 3.2)
project(fury VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set(name app)

add_subdirectory(third-parties/glfw)
include_directories(include)
include_directories(third-parties/glfw/include)
include_directories(third-parties/glad/include)
include_directories(third-parties/stb/include)

add_executable(
        app

        third-parties/glad/src/glad.c

        source/mem/alloc.c
        source/mem/rbt.c
        source/mem/buddy.c
        source/mem/memory.c
        source/mem/slab.c
        source/mem/std.c
        source/mem/arena.c
        source/mem/stack.c
        source/mem/pool.c
        source/mem/freelist.c

        source/shader.c
        source/draw.c
        source/game.c
        source/grid.c
        source/editor.c
        source/camera.c
        source/input.c
        source/debug.c
        source/file.c
        source/mesh.c
        source/sort.c
        source/benchmark.c

        source/engine/Component.cpp
        src/internal/main.cpp
)

target_link_libraries(app glfw)

