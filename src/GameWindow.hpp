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

#include "engine/Object.hpp"
#include "data/Heap.hpp"
#include "data/ProbeHashTable.hpp"
#include "data/DrawUtils.hpp"

struct GameWindow : public Object<GameWindow> {

    inline void Create() {
    }

    inline void Update() {
    }
};
