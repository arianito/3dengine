#pragma once

#include <memory>


#include "engine/CLevelManager.hpp"

#include "Fluid/FluidSim.hpp"


int reverseExists(const std::vector<std::string>& arr) {
    int ans = 0;
    std::unordered_map<std::string, int> map;
    int n = arr.size();
    for(int i = 0; i < n; i++) {
        std::string tmp{arr[i]};
        std::reverse(tmp.begin(), tmp.end());
        map[tmp] = i;
    }
    for (int i = 0; i < n; i++) {
        const auto& found = map.find(arr[i]);
        if(found != map.end() && found->second != i) {
            ans ++;
        }
    }
    return ans / 2;
}

struct GameWindow {
    CLevelManager<> manager;
    bool debug = true;

    inline void Create() {
        manager.Add<FluidSim>();
        manager.Load<FluidSim>();

//        printf("%d\n", reverseExists({"cd","ac","dc","ca","zz"}));
//        printf("%d\n", reverseExists({"ac", "dc","bc"}));
//        printf("%d\n", reverseExists({"ac", "ca"}));
//        printf("%d\n", reverseExists({"cd", "cd", "dc", "dc"}));
    }

    inline void Update() {
        manager.Update();

        if (input_keydown(KEY_TAB)) {
            debug ^= 1;
        }

        if (debug) {
            debug_origin(vec2(0, 1));
            debug_color(color_yellow);
            Vec2 pos = vec2(10, game->height - 10);
            debug_stringf(pos, "boot %d / %d\n"
                               "global %d / %d\n"
                               "freelist %d / %d\n"
                               "slab %d / %d\n"
                               "string %d / %d\n"
                               "buddy %d / %d\n"
                               "stack %d / %d",
                          alloc->boot->usage, alloc->boot->total,
                          alloc->global->usage, alloc->global->total,
                          freelist_usage(alloc->freelist), alloc->freelist->total,
                          alloc->slab->usage, alloc->slab->total,
                          freelist_usage(alloc->string), alloc->string->total,
                          alloc->buddy->usage, alloc->metadata.buddy,
                          alloc->stack->usage, alloc->stack->total
            );
        }
    }
};
