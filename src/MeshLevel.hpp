#pragma once

#include <string_view>
#include "engine/CLevelManager.hpp"
#include "engine/TVector.hpp"
#include "engine/TVector4.hpp"
#include "engine/TMatrix.hpp"
#include "engine/CMesh.hpp"
#include "engine/Trace.hpp"

class MeshAlloc {

public:
    static FreeListMemory *memory;

    static inline void create() {
        memory = make_freelist(5 * MEGABYTES);
    }

    static inline void destroy() {
        freelist_destroy(&memory);
    }

    inline static void *Alloc(size_t size, unsigned int alignment) {
        return freelist_alloc(memory, size, alignment);
    }

    inline static void Free(void **ptr) {
        freelist_free(memory, ptr);
    }

    inline static size_t usage() {
        return freelist_usage(memory);
    }

    inline static size_t size() {
        return memory->total;
    }

};

FreeListMemory *MeshAlloc::memory = nullptr;

class MeshLevel : public CLevel {
    using TAlloc = MeshAlloc;

    CMeshGroup<TAlloc> *group;

    inline void Create() override {
        TAlloc::create();
        group = CWavefrontOBJ<TAlloc>::Load("models/box.obj");
    }

    inline void Update() override {

        debug_origin(vec2(1, 1));
        debug_color(color_white);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "Alloc: %zu / %zu", TAlloc::usage(), TAlloc::size());

        if (input_keydown(KEY_SPACE) && group == nullptr) {
            group = CWavefrontOBJ<TAlloc>::Load("models/box.obj");
        }
        if (input_keydown(KEY_M) && group != nullptr) {
            Free<TAlloc>(&group);
        }
    }

    inline void Destroy() override {
        TAlloc::destroy();
    }
};