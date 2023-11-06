#pragma once

#include <memory>
#include "engine/CMesh.hpp"
#include "engine/CLevelManager.hpp"
#include "engine/mathf.hpp"

extern "C" {
#include "noise.h"
#include "shader.h"
}

class MeshAlloc {

public:
    static P2SlabMemory *memory;

    static inline void create() {
        memory = make_p2slab(10);
    }

    static inline void destroy() {
        p2slab_destroy(&memory);
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return p2slab_alloc(memory, size);
    }

    inline static void Free(void **ptr) {
        p2slab_free(memory, ptr);
    }

    inline static size_t usage() {
        return memory->usage;
    }

    inline static size_t size() {
        return memory->total;
    }

    inline static void fit() {
        p2slab_fit(memory);
    }

};

P2SlabMemory *MeshAlloc::memory = nullptr;

struct Temp : public CLevel {
    using TAlloc = MeshAlloc;
    CMeshGroup<TAlloc> *mesh;
    Shader frac_shader;

    void Create() override {
        TAlloc::create();

        frac_shader = shader_load("shaders/frac.vs", "shaders/frac.fs");
        mesh = CWavefrontOBJ<TAlloc>::Load("models/plane2.obj");

        for (const auto &item: mesh->Meshes) {
            item->Prepare();
        }
    }

    void Update() override {
        shader_begin(frac_shader);
        shader_mat4(frac_shader, "viewProjection", &camera->viewProjection);
        shader_mat4(frac_shader, "model", &mat4_identity);
        shader_float(frac_shader, "time", gameTime->time * 0.5f);

        Ray r = camera_screenToWorld(input->position);
        Vec3 pos = vec3_intersectPlane(r.origin, r.origin + r.direction * 100.0f, vec3_zero, vec3_up);
        pos /= 200.0f;

        Vec2 mousePos{pos.x, pos.y};
        shader_vec2(frac_shader, "mouse", &mousePos);

        for (const auto &item: mesh->Meshes)
            item->Render();

        shader_end();
    }

    void Destroy() override {
        shader_destroy(frac_shader);
        Free<TAlloc>(&mesh);
        TAlloc::destroy();
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
