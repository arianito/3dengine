#pragma once

#include <memory>

extern "C"
{
#include "draw.h"
#include "game.h"
#include "mathf.h"
#include "mem/freelist.h"
#include "mem/utils.h"
}

#include "data/Array.hpp"
#include "engine/Object.hpp"
#include "engine/Memory.hpp"

struct GameWindow : public Object<GameWindow> {
    inline void Create() {
    }

    inline void Update() {

        draw_arrow(vec3_zero,
                   vec3(100.0f, 0, 0),
                   vec3(1,0,1),
                   color_red,
                   20);
        draw_arrow(vec3_zero,
                   vec3(0, 0, 100.0f),
                   vec3(0,1,1),
                   color_blue,
                   20);

        draw_arrow(vec3_zero,
                   vec3(0, 100.0f, 0),
                   vec3(1,1,0),
                   color_green,
                   20);


        draw_axis(vec3(100, 100, 0), quat(gameTime->time * 50.0f, 0, 0), 100);
    }
};