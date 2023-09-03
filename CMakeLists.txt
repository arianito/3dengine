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
    
    source/draw.c
    source/alloc.c
    source/game.c
    source/grid.c
    source/shader.c
    source/editor.c
    source/input.c
    source/debug.c
    source/file.c

    source/mem.c
    source/sort.c
    source/benchmark.c

    src/main.c
)
target_link_libraries(app glfw)

