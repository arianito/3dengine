#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
#include "sort.h"
#include "mem/buddy.h"
}

#include "engine/Level.hpp"

static constexpr int nPool = 1000;

class BuddyLevel : public Level {

    inline void Create() override {
    }

    inline void Update() override {
    }

    inline void Destroy() override {
    }
};