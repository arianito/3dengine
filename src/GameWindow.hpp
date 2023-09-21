#pragma once

#include <memory>


#include "engine/LevelManager.hpp"
#include "./StartLevel.hpp"
#include "./BuddyLevel.hpp"


struct GameWindow {
    LevelManager<> manager;

    inline void Create() {
        manager.Add<StartLevel>();
        manager.Add<BuddyLevel>();

        manager.Load<BuddyLevel>();
    }

    inline void Update() {
        manager.Update();

        if (input_keydown(KEY_0)) {
            manager.Load<StartLevel>();
        }
        if (input_keydown(KEY_9)) {
            manager.Load<BuddyLevel>();
        }
    }
};
