#pragma once

extern "C" {
#include "shader.h"
}

#include "ctime"
#include <unordered_map>
#include "engine/CLevelManager.hpp"
#include "data/hash.hpp"
#include "data/TFlatMap.hpp"
#include "data/TFastMap.hpp"
#include "engine/mathf.hpp"
#include "engine/Trace.hpp"

#include <immintrin.h>
#include "engine/CMesh.hpp"


class GraphLevel : public CLevel {

    class CustomStartLevelAllocator {
    public:
        static FreeListMemory *memory;

        inline static void *Alloc(size_t size, unsigned int alignment) {
            return freelist_alloc(memory, size, alignment);
        }

        inline static void Free(void **ptr) {
            freelist_free(memory, ptr);
        }
    };


    using Map = TFastMap<Vec3, std::pair<Color, bool>, CustomStartLevelAllocator>;
    Map *map;
    bool camMode = false;
    CMeshGroup<CustomStartLevelAllocator> *group;
    Shader phongShader;

    void Create() override {
        CustomStartLevelAllocator::memory = make_freelist(128 * MEGABYTES);
        map = AllocNew<CustomStartLevelAllocator, Map>();
        group = CWavefrontOBJ<CustomStartLevelAllocator>::Load("models/cube.obj");
        phongShader = shader_load("shaders/default.vs", "shaders/default.fs");
        prepareMesh(group->Meshes[0]);
    }

    void Update() override {
        debug_origin(Vec2{0, 0});
        debug_color(color_white);

        Ray ray = camera_screenToWorld(input->position);
        Vec3 world = ray.origin + ray.direction * (camera->zoom / 10000.0f);
        world = vec3_intersectPlane(ray.origin, ray.origin + ray.direction, vec3_zero, vec3_up);


        if (input_keydown(KEY_Z)) {
            camMode ^= 1;
        }

        float speed = 120.0f;

        if (input_mousepress(MOUSE_LEFT)) {
            int n = 5;
            for (int i = -n; i <= n; i++) {
                Vec3 p = world + vec3_rand(300, 300, 0) + Vec3{0, 0, 1000.0f};
                p = vec3_snapCube(p, 20);
                map->Set(p, std::make_pair(
                        color_alpha(color_lerp(color_green, color_yellow, randf()), randf() + 0.1f),
                        true
                ));
            }
        }

        if (input_keydown(KEY_M)) {

            for (const auto &node: *map) {
                if(node->value.second) {
                    map->Remove(node->key);
                }
            }
        }

        if(camMode) {
            camera->rotation = rot(-30, gameTime->time * 30.0f, 0);
            camera->position = rot_forward(camera->rotation) * -camera->zoom;
            camera_update();
        }
        float t = gameTime->time;

        float i = 0;
        for (const auto &node: *map) {
            Vec3 p = node->key;
            Color color = node->value.first;
            bool mv = node->value.second;

            float mg = vec3_mag(p);
            float d = 6;
            const auto n = (float)map->Length();
            if(mv) {
                d = (cosd(mg * 0.5f + (i / n) * 1000.0f) + 0.8f) * 2.5f + 0.5f;
                p.z = tand(t * 200.0f + (i / n) * 1000.0f) * 50.0f;
                p.x += sind(t * 200.0f + i) * 20.0f;
                p.y += cosd(t * 200.0f + i) * 20.0f;
                d *= clamp01(1000.0f / mg) * 5.0f;

                p.z += d * 10.0f;
                p = rot_rotate(Rot{0, sind(mg * 0.1f * t) * 30.0f, 0}, p);

            }
            draw_point(p, d + (1.3f - color.a) * 1.0f, color);
            i += 1.0f;
        }

        debug_stringf(Vec2{10, 20}, "map: %d / %d", map->Length(), map->Capacity());

        debug_origin(vec2(1, 1));
        debug_color(color_yellow);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "temp %d / %d", freelist_usage(CustomStartLevelAllocator::memory), CustomStartLevelAllocator::memory->total);


        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);

        Vec3 lightPos{0, 0, 0};
        Vec3 lightColor{color_green.r, color_green.g, color_green.b};
        Vec3 objectColor{lightColor};


        shader_begin(phongShader);
        shader_vec3(phongShader, "lightPos", &lightPos);
        shader_vec3(phongShader, "lightColor", &lightColor);
        shader_vec3(phongShader, "objectColor", &objectColor);
        shader_vec3(phongShader, "viewPos", &camera->position);
        shader_mat4(phongShader, "viewProjection", &camera->viewProjection);

        Mat4 world2 = mat4_scalef(120);
        shader_mat4(phongShader, "model", &world2);
        glBindVertexArray(modelVAO);
        glDrawElements(GL_TRIANGLES, group->Meshes[0]->Indices.Length(), GL_UNSIGNED_INT, nullptr);

        shader_end();
    }

    void Destroy() override {
        shader_destroy(phongShader);
        deleteMesh();
        freelist_destroy(&CustomStartLevelAllocator::memory);
    }


    //
    GLuint modelVAO, modelVBO, modelEBO;

    template<class T>
    void prepareMesh(CMesh<T> *mesh) {

        glGenVertexArrays(1, &modelVAO);
        glGenBuffers(1, &modelVBO);
        glGenBuffers(1, &modelEBO);

        glBindVertexArray(modelVAO);
        glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.Length() * sizeof(TMeshVertex), mesh->Vertices.Ptr(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.Length() * sizeof(int), mesh->Indices.Ptr(), GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TMeshVertex), (void *) BUFFER_OFFSET(0));
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TMeshVertex), (void *) BUFFER_OFFSET(16));
        // vertex normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TMeshVertex), (void *) offsetof(TMeshVertex, TexCoord));

        glBindVertexArray(0);
    }

    void deleteMesh() {
        glDeleteVertexArrays(1, &modelVAO);
        glDeleteBuffers(1, &modelVBO);
        glDeleteBuffers(1, &modelEBO);
    }

};

FreeListMemory *GraphLevel::CustomStartLevelAllocator::memory = nullptr;