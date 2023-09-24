#pragma once

extern "C" {
#include "mathf.h"
#include "debug.h"
#include "sort.h"
#include "mesh.h"
#include "mem/buddy.h"
}

#include "engine/Level.hpp"

class MeshLevel : public Level {

    inline void Create() override {
        Mesh* m = mesh_obj("models/box.obj");
    }

    inline void Update() override {
    }

    inline void Destroy() override {
    }
};