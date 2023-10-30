#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
}

#include "engine/CLevelManager.hpp"
#include "engine/ECS.hpp"
#include "engine/Memory.hpp"
#include "data/hash.hpp"
#include "engine/mathf.hpp"

using TAlloc = BuddyMemory;

template<int size>
struct Conway {
    char board[size][size]{0};

    void step() {
        for (int j = 0; j < size; j++) {
            for (int i = 0; i < size; i++) {
                bool alive = board[j][i] & 1;
                int cnt = count(i, j);
                if (alive && (cnt < 2 || cnt > 3)) {
                    board[j][i] |= 4;
                }
                if (!alive && cnt == 3) {
                    board[j][i] |= 8;
                }
            }
        }
        for (int j = 0; j < size; j++) {
            for (int i = 0; i < size; i++) {
                if ((board[j][i] & 4) == 4)
                    board[j][i] = 0;
                else if ((board[j][i] & 8) == 8)
                    board[j][i] = 1;
            }
        }
    }

    int kernels[8][2] = {
            -1, -1,
            -1, 0,
            -1, 1,
            0, -1,
            0, 1,
            1, -1,
            1, 0,
            1, 1,

    };
    int count(int x, int y) {
        int cnt = 0;
        for(auto & kernel : kernels) {
            int i = kernel[0] + x;
            int j = kernel[1] + y;
            if(i >= 0 && i < size && j >= 0 && j < size)
                if (board[j][i] & 1) cnt++;
        }

        return cnt;
    }
};

enum {
    n = 100,
};

class ConwaysGameOfLife : public CLevel {
    Conway<n> *conways;
    float lastStep = 0;

    void Create() override {
        conways = AllocNew<TAlloc, Conway<n>>();
    }

    void Update() override {
        int i, j;

        Ray r = camera_screenToWorld(input->position);
        Vec3 world = vec3_intersectPlane(r.origin, r.origin + r.direction, vec3_zero, vec3_up);

        i = clamp(world.x / 10.0f + n / 2.0f, 0, n);
        j = clamp(world.y / 10.0f + n / 2.0f, 0, n);

        if (input_mousepress(MOUSE_LEFT)) {
            conways->board[j][i] = 1;
        }

        Vec3 pos{};
        debug_origin(Vec2{0.5, 0.5});
        debug_rotation(-camera->rotation);
        for (j = 0; j < n; j++) {
            for (i = 0; i < n; i++) {
                pos.x = ((float) i - n / 2.0f) * 10.0f;
                pos.y = ((float) j - n / 2.0f) * 10.0f;
                pos.z = 0;
                if (conways->board[j][i])
                {
                    draw_cubef(pos, 5.0f, color_green);
                } else {
                    draw_point(pos, 0.25f, color_green);
                }
            }
        }

        if (gameTime->time - lastStep > 0.1f) {
            conways->step();
            lastStep = gameTime->time;
        }
//        debug_origin(Vec2{0, 0});
//        debug_stringf(Vec2{10, 20}, "map: %d / %d", particles->Length(), particles->Capacity());
    }

    void Destroy() override {
        Free<TAlloc>(&conways);
    }
};