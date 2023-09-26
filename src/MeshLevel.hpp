#pragma once

extern "C" {
#include "shader.h"
}

#include <cstddef>

#include <string_view>
#include "engine/CLevelManager.hpp"
#include "engine/TVector.hpp"
#include "engine/TVector4.hpp"
#include "engine/TMatrix.hpp"
#include "engine/CMesh.hpp"
#include "engine/Trace.hpp"

#define BUFFER_OFFSET(x) ((const void *)(x))

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
    TArray<Vec3> arr;
    Shader sh;
    GLuint VAO, VBO, EBO;
    int nIndices;


    inline void Create() override {
        TAlloc::create();

        for (int i = 0; i < 100; i++) {
            arr.Add(vec3_rand(200, 200, 200));
        }

        sh = shader_load("shaders/default.vs", "shaders/default.fs");

        group = CWavefrontOBJ<TAlloc>::Load("models/cube.obj");
        prepareMesh(group->Meshes[0]);
        nIndices = group->Meshes[0]->Indices.Length();
    }

    inline void Update() override {

        debug_origin(vec2(1, 1));
        debug_color(color_white);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "Alloc: %zu / %zu", TAlloc::usage(), TAlloc::size());

        if (input_keydown(KEY_SPACE) && group == nullptr) {
            group = CWavefrontOBJ<TAlloc>::Load("models/cars.obj");
            deleteMesh();
            prepareMesh(group->Meshes[0]);
            nIndices = group->Meshes[0]->Indices.Length();
        }
        if (input_keydown(KEY_M) && group != nullptr) {
            Free<TAlloc>(&group);
            deleteMesh();
        }
        if (input_keydown(KEY_G)) {
            arr.Clear();
            for (int i = 0; i < 100; i++) {
                arr.Add(vec3_rand(200, 200, 50));
            }
        }

//        if (group != nullptr) {
//            Vertex v;
//            v.color = color_red;
//            v.size = 0.2f;
//            auto &indices = group->Meshes[0]->Indices;
//            auto &vertices = group->Meshes[0]->Vertices;
//            for (int i = 0; i < indices.Length() - 3; i += 3) {
//                int index = indices[i];
//                Vec3 v1 = vec3_mulf(vertices[index + 0].Position, 20);
//                Vec3 v2 = vec3_mulf(vertices[index + 1].Position, 20);
//                Vec3 v3 = vec3_mulf(vertices[index + 2].Position, 20);
//
//                draw_triangle(triangle(v1, v2, v3), color_red);
//            }
//        }

        if (VAO) {
            shader_begin(sh);

            Vec3 lightPos = vec3(200, 200, 200);

            shader_vec3(sh, "lightPos", &lightPos);
            shader_vec3(sh, "viewPos", &camera->position);
            Vec3 lightColor = vec3_one;
            Vec3 objectColor = vec3(0.76f, 0.89f, 0.71f);
            shader_vec3(sh, "lightColor", &lightColor);
            shader_vec3(sh, "objectColor", &objectColor);

            shader_mat4(sh, "projection", &camera->projection);
            shader_mat4(sh, "view", &camera->view);
            Mat4 world = mat4_scalef(20);
            shader_mat4(sh, "model", &world);

            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, nullptr);
            glDisable(GL_DEPTH_TEST);
        }


        shader_end();
    }

    inline void Destroy() override {
        TAlloc::destroy();
        shader_destroy(sh);
    }


    template<class T>
    void prepareMesh(CMesh<T> *mesh) {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.Length() * sizeof(TMeshVertex), mesh->Vertices.Ptr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.Length() * sizeof(int), mesh->Indices.Ptr(), GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMeshVertex), (void *) BUFFER_OFFSET(0));
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TMeshVertex), (void *) BUFFER_OFFSET(16));

        glBindVertexArray(0);
    }

    void deleteMesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = 0;
    }
};