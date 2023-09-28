#pragma once

extern "C" {
#include "shader.h"
#include "mem/p2slab.h"
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

class MeshLevel : public CLevel {
    using TAlloc = MeshAlloc;
    CMeshGroup<TAlloc> *group;
    Shader phongShader;
    Shader depthShader;
    Shader bufferShader;


    inline void Create() override {
        TAlloc::create();

        group = CWavefrontOBJ<TAlloc>::Load("models/simple.obj");

        phongShader = shader_load("shaders/default.vs", "shaders/default.fs");
        depthShader = shader_load("shaders/depth.vs", "shaders/depth.fs");
        bufferShader = shader_load("shaders/rr.vs", "shaders/rr.fs");

        prepareMesh(group->Meshes[0]);
        prepareFrame();
        prepareQuad();
    }

    inline void Update() override {
        debug_origin(vec2_one);
        debug_color(color_white);
        Vec2 pos = vec2(game->width - 10, game->height - 10);
        debug_stringf(pos, "Alloc: %zu / %zu", TAlloc::usage(), TAlloc::size());

        if (input_keydown(KEY_SPACE)) {
            for (const auto &m: group->Meshes) {
                m->Vertices.Fit();
                m->Indices.Fit();
            }
            group->Meshes.Fit();
            TAlloc::fit();
        }
        if (group != nullptr) {

            Mat4 world = mat4_scalef(40);

            Vec3 lightColor = vec3_one;
            Vec3 objectColor = vec3(0.87f, 0.21f, 0.54f);

            float t = gameTime->time * 30.0f;

            Vec3 lightPos = vec3(cosd(t) * 500.0f, sind(t) * 500.0f, 200.0f);

            Rot lightRot = rot_lookAt(lightPos, vec3_zero, vec3_one);
            float zoom = camera->zoom;

            Mat4 lightView = mat4_view(lightPos, lightRot);
            Mat4 lightProj = mat4_orthographic(-zoom, zoom, -zoom, zoom, 1.0f, 500);
            Mat4 depthMVP = mat4_mul(lightView, lightProj);

            //
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
            // Render to our framebuffer
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            shader_begin(depthShader);
            shader_mat4(depthShader, "viewProjection", &depthMVP);
            renderMesh(depthShader, world);
            shader_end();
            //
            glViewport(0, 0, (int) game->width, (int) game->height);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //
            shader_begin(phongShader);
            shader_vec3(phongShader, "lightPos", &lightPos);
            shader_vec3(phongShader, "lightColor", &lightColor);
            shader_vec3(phongShader, "objectColor", &objectColor);
            shader_vec3(phongShader, "viewPos", &camera->position);
            shader_mat4(phongShader, "viewProjection", &camera->viewProjection);
            shader_mat4(phongShader, "depthViewProjection", &depthMVP);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthTex);
            renderMesh(phongShader, world);
            shader_end();
        }

    }

    inline void Destroy() override {
        deleteMesh();
        deleteFrame();
        deleteQuad();
        shader_destroy(phongShader);
        shader_destroy(depthShader);
        shader_destroy(bufferShader);
        Free<TAlloc>(&group);
        TAlloc::destroy();
    }

    void renderMesh(Shader sh, const Mat4 &space) const {
        int n = group->Meshes[0]->Indices.Length();
        int c = 2;
        float ofc = 200.0f;
        float scale = 15.0f;
        for (int i = -c; i <= c; i++) {

            for (int j = -c; j <= c; j++) {
                auto k = (float) (j * c + i);
                Mat4 world = mat4_transformf(vec3((float) j * ofc, (float) i * ofc, sin(k * 7) * 0), scale);
                shader_mat4(sh, "model", &world);
                glBindVertexArray(modelVAO);
                glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    void renderQuad() {
        Mat4 world = mat4_scalef(20);
        shader_begin(bufferShader);
        shader_mat4(bufferShader, "projection", &camera->projection);
        shader_mat4(bufferShader, "view", &camera->view);
        shader_mat4(bufferShader, "model", &world);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        shader_end();
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (const float *) &color_white);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        float zo = -10.0f;
        Quad vertices[] = {
                {vec3(-w, -w + zo, 0), vec3(0, 0, 1), vec2(0, 0)},
                {vec3(-w, +w + zo, 0), vec3(0, 0, 1), vec2(1, 0)},
                {vec3(+w, +w + zo, 0), vec3(0, 0, 1), vec2(1, 1)},
                {vec3(+w, -w + zo, 0), vec3(0, 0, 1), vec2(0, 1)},
        };
        unsigned int indices[] = {2, 1, 0, 3, 2, 0};

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