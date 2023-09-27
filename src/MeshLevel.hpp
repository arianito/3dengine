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
    Shader phongShader;
    Shader depthShader;
    Shader bufferShader;



    inline void Create() override {
        TAlloc::create();

        group = CWavefrontOBJ<TAlloc>::Load("models/cube.obj");

        phongShader = shader_load("shaders/default.vs", "shaders/default.fs");
        depthShader = shader_load("shaders/depth.vs", "shaders/depth.fs");
        bufferShader = shader_load("shaders/rr.vs", "shaders/rr.fs");

        prepareMesh(group->Meshes[0]);
        prepareFrame();
        prepareQuad();
    }

    inline void Update() override {
        debug_origin(vec2(1, 1));
        debug_color(color_white);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "Alloc: %zu / %zu", TAlloc::usage(), TAlloc::size());

        if (group != nullptr) {
            int n = group->Meshes[0]->Indices.Length();

            Mat4 world = mat4_scalef(20);

            Vec3 lightColor = vec3_one;
            Vec3 objectColor = vec3(0.76f, 0.89f, 0.71f);

            Vec3 lightPos = vec3(cosd(gameTime->time * 50.0f) * 500.0f, sind(gameTime->time * 50.0f) * 500.0f, 260);
            Rot lightRot = rot_lookAt(lightPos, vec3_zero, vec3_up);


            Vec3 forward = rot_forward(lightRot);
            Mat4 lightView = mat4_lookAt(vec3_neg(forward), vec3_zero, vec3_up);
            Mat4 lightProj = mat4_orthographic(-40, 40, -40, 40, -40, 1000);
            Mat4 depthMVP = mat4_mul(lightProj, lightView);

            draw_axisRot(lightPos, 100, lightRot);

            // Render to our framebuffer
            shader_begin(depthShader);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CW);
            shader_mat4(depthShader, "projection", &lightProj);
            shader_mat4(depthShader, "view", &lightView);
            shader_mat4(depthShader, "model", &world);
            glBindVertexArray(modelVAO);
            glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, nullptr);
            shader_end();
            //
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, (int) game->width, (int) game->height);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthTex);
            //
            shader_begin(phongShader);
            shader_vec3(phongShader, "lightPos", &lightPos);
            shader_vec3(phongShader, "viewPos", &camera->position);
            shader_vec3(phongShader, "lightColor", &lightColor);
            shader_vec3(phongShader, "objectColor", &objectColor);
            shader_mat4(phongShader, "projection", &camera->projection);
            shader_mat4(phongShader, "view", &camera->view);
            shader_mat4(phongShader, "model", &world);
            shader_int(phongShader, "shadowMap", 0);
            shader_mat4(phongShader, "depthBias", &depthMVP);
            glBindVertexArray(modelVAO);
            glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, nullptr);
            shader_end();
            //
            shader_begin(bufferShader);
            shader_mat4(bufferShader, "projection", &camera->projection);
            shader_mat4(bufferShader, "view", &camera->view);
            shader_mat4(bufferShader, "model", &world);
            shader_int(bufferShader, "shadowMap", 0);
            glBindVertexArray(quadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            shader_end();
            //
        }

    }

    inline void Destroy() override {
        deleteMesh();
        deleteFrame();
        shader_destroy(phongShader);
        shader_destroy(depthShader);
        shader_destroy(bufferShader);
        Free<TAlloc>(&group);
        TAlloc::destroy();
    }


    //
    GLuint depthFBO, depthTex;
    const GLsizei SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    void prepareFrame() {
        glGenFramebuffers(1, &depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
        glDrawBuffer(GL_NONE); // No color buffer is drawn to.

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            exit(1);
        }

    }

    void deleteFrame() {
        glDeleteFramebuffers(1, &depthFBO);
        glDeleteTextures(1, &depthTex);
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

    GLuint quadVAO, quadVBO, quadEBO;

    void prepareQuad() {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &quadEBO);

        float w = 5.0f;
        float zo = 5.0f;
        Quad vertices[] = {
                {vec3(5, -w, -w+zo),   vec3(-1, 0, 0), vec2(0, 0)},
                {vec3(5, -w, w+zo),  vec3(-1, 0, 0), vec2(0, 1)},
                {vec3(5, w, w+zo), vec3(-1, 0, 0), vec2(1, 0)},
                {vec3(5, w, -w+zo),  vec3(-1, 0, 0), vec2(1, 1)},
        };
        unsigned int indices[] = {0, 1, 2, 0, 2, 3};

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(16));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), BUFFER_OFFSET(32));
    }

    void deleteQuad() {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &quadEBO);
    }

    void wireframe() {
        if (group != nullptr) {
            Vertex v;
            v.color = color_red;
            v.size = 0.2f;
            auto &indices = group->Meshes[0]->Indices;
            auto &vertices = group->Meshes[0]->Vertices;
            for (int i = 0; i < indices.Length() - 3; i += 3) {
                int index = indices[i];
                Vec3 v1 = vec3_mulf(vertices[index + 0].Position, 20);
                Vec3 v2 = vec3_mulf(vertices[index + 1].Position, 20);
                Vec3 v3 = vec3_mulf(vertices[index + 2].Position, 20);

                draw_triangle(triangle(v1, v2, v3), color_black);
            }
        }
    }
};