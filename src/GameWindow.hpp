#pragma once

#include <memory>
#include "engine/CMesh.hpp"
#include "engine/CLevelManager.hpp"
#include "engine/mathf.hpp"

extern "C" {
#include "noise.h"
#include "shader.h"
}

template<class TAlloc = FreeListMemory>
class QuadTree {
private:
    BBox boundary;
    TArray<Vec3, TAlloc> points{4};
    QuadTree *blocks[8]{nullptr};
public:
    explicit QuadTree(BBox bounds) : boundary{bounds} {}

    ~QuadTree() {
        if (blocks[0]) {
            for (auto &block: blocks) {
                Free<TAlloc>(&block);
            }
        }
    }

    bool Add(const Vec3 &pos) {
        if (!bbox_containsPoint(boundary, pos)) return false;

        if (!blocks[0]) {
            if (points.Length() < 4) {
                points.Add(pos);
                return true;
            }
            subdivide();
        }

        for (auto &block: blocks) {
            if (block->Add(pos)) return true;
        }

        return false;
    }

    void draw() {
        draw_bbox(boundary, color_gray);
        if (!blocks[0]) return;

        for (auto &block: blocks) {
            block->draw();
        }
    }

private:
    void subdivide() {
        Vec3 halfSize = bbox_size(boundary) * 0.5f;
        Vec3 start;

        start = boundary.min + Vec3{0, 0, 0};
        blocks[0] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{0, halfSize.y, 0};
        blocks[1] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{0, 0, halfSize.z};
        blocks[2] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{0, halfSize.y, halfSize.z};
        blocks[3] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});

        start = boundary.min + Vec3{halfSize.x, 0, 0};
        blocks[4] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{halfSize.x, halfSize.y, 0};
        blocks[5] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{halfSize.x, 0, halfSize.z};
        blocks[6] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
        start = boundary.min + Vec3{halfSize.x, halfSize.y, halfSize.z};
        blocks[7] = AllocNew<TAlloc, QuadTree>(BBox{start, start + halfSize});
    }

};

struct Temp : public CLevel {
    TArray<Vec3, SlabMemory> arr;
    QuadTree<SlabMemory> quad{
            BBox{
                    Vec3{-100, -100, -100},
                    Vec3{100, 100, 100}
            }
    };

    void Create() override {
    }

    void Update() override {
        for (const auto &pos: arr) {
            float r = vec3_mag(pos);
            draw_point(pos, 0.01f, color_red);
        }

        if (input_mousepress(MOUSE_LEFT)) {
            Ray r = camera_screenToWorld(input->position);
            Vec3 mouse = vec3_intersectPlane(r.origin, r.origin + r.direction * 10, vec3_zero, vec3_up);
            Vec3 p = mouse + vec3_rand(5, 5, 5);

            arr.Add(p);
            quad.Add(p);
        }
        quad.draw();
    }

    void Destroy() override {
    }
};

struct GameWindow {
    CLevelManager<> manager;
    bool debug = true;

    inline void Create() {
        manager.Add<Temp>();
        manager.Load<Temp>();
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
