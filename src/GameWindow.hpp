#pragma once

#include <memory>


#include "engine/LevelManager.hpp"
#include "./StartLevel.hpp"


struct GameWindow {
    LevelManager<> manager;

    inline void Create() {
        manager.Add<StartLevel>("start");

        manager.Load("start");
    }

    inline void Update() {
        manager.Update();
    }
};
