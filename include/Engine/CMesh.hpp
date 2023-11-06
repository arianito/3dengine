#pragma once

extern "C" {
#include "mesh.h"
}

#include <iostream>
#include "engine/TVector.hpp"
#include "engine/Trace.hpp"
#include "data/TArray.hpp"

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

struct __attribute__((aligned(64), packed)) TMeshVertex {
    Vec3 Position{};
    Vec3 Normal{};
    Vec2 TexCoord{};
};

struct CMaterial {
    Vec3 Ambient{};
    Vec3 Diffuse{};
    Vec3 Specular{};
    float SpecularAmount{};
    float Opacity{};
};

template<class TAlloc>
struct CMesh {
    TString<TAlloc> Name;
    TArray<TMeshVertex, TAlloc> Vertices;
    TArray<int, TAlloc> Indices;

    explicit inline CMesh() = default;

    explicit inline CMesh(const CMesh &) = delete;

    inline ~CMesh();

    inline void Prepare();

    inline void Render();

private:
    GLuint modelVAO, modelVBO, modelEBO;
};

template<class TAlloc>
void CMesh<TAlloc>::Prepare() {

    glGenVertexArrays(1, &modelVAO);
    glGenBuffers(1, &modelVBO);
    glGenBuffers(1, &modelEBO);

    glBindVertexArray(modelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.Length() * sizeof(TMeshVertex), Vertices.Ptr(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.Length() * sizeof(int), Indices.Ptr(), GL_STATIC_DRAW);

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
    printf(" -- %d \n", modelVAO);
}

template<class TAlloc>
void CMesh<TAlloc>::Render() {
    if (!modelVAO)return;
    glBindVertexArray(modelVAO);
    glDrawElements(GL_TRIANGLES, Indices.Length(), GL_UNSIGNED_INT, nullptr);
}

template<class TAlloc>
CMesh<TAlloc>::~CMesh() {
    if (!modelVAO) return;
    glDeleteVertexArrays(1, &modelVAO);
    glDeleteBuffers(1, &modelVBO);
    glDeleteBuffers(1, &modelEBO);
}


template<class TAlloc>
struct CMeshGroup {

    TString<TAlloc> Name;
    TArray<CMesh<TAlloc> *, TAlloc> Meshes;

    explicit inline CMeshGroup() = default;

    explicit inline CMeshGroup(const CMeshGroup &) = delete;

    inline ~CMeshGroup() {
        for (auto mesh: Meshes) {
            Free<TAlloc>(&mesh);
        }
    }

};

template<class TAlloc>
class CWavefrontOBJ {
public:

    static inline CMeshGroup<TAlloc> *Load(const char *path) {
        auto group = AllocNew<TAlloc, CMeshGroup<TAlloc>>();

        char *line;
        long cursor = 0;
        FILE *f = nullptr;

        fopen_s(&f, resolve_stack(path), "r");
        stack_pop(alloc->stack);
        CMesh<TAlloc> *mesh;


        if (f != nullptr) {
            TArray<Vec3, TAlloc> positions;
            TArray<Vec3, TAlloc> normals;
            TArray<Vec2, TAlloc> coords;
            while ((line = readline_stack(f, &cursor)) != nullptr) {
                auto token = firstToken(line);
                if (token == "o") {
                    auto name = lastToken(line);
                    mesh = AllocNew<TAlloc, CMesh<TAlloc>>();
                    mesh->Name = name;
                    group->Meshes.Add(mesh);
                }

                if (token == "v") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 3) positions.Add(vec3(stof(split[0]), stof(split[1]), stof(split[2])));
                    stack_free(alloc->stack, (void **) &split);
                } else if (token == "vn") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 3) normals.Add(vec3(stof(split[0]), stof(split[1]), stof(split[2])));
                    stack_free(alloc->stack, (void **) &split);
                } else if (token == "vt") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 2) coords.Add(vec2(stof(split[0]), stof(split[1])));
                    stack_free(alloc->stack, (void **) &split);
                } else if (token == "f") {
                    TStringView *faces = split_stack(lastToken(line), ' ');
                    unsigned int nFaces = stack_n(alloc->stack) / sizeof(TStringView);
                    {
                        TMeshVertex *vertices;
                        if (!(vertices = generateVertices_stack(faces, nFaces, positions, normals, coords))) {
                            stack_free(alloc->stack, (void **) &vertices);
                            stack_free(alloc->stack, (void **) &faces);
                            stack_free(alloc->stack, (void **) &line);
                            Free<TAlloc>(&group);
                            return nullptr;
                        }

                        unsigned int nVertices = stack_n(alloc->stack) / sizeof(TMeshVertex);

                        for (int i = 0; i < nVertices; i++) {
                            mesh->Vertices.Add(vertices[i]);
                        }

                        int *indices;
                        if ((indices = triangulate_stack(vertices, nVertices))) {
                            unsigned int nIndices = stack_n(alloc->stack) / sizeof(int);
                            for (int i = 0; i < nIndices; i++) {
                                int ind = (mesh->Vertices.Length() - nVertices) + indices[i];
                                mesh->Indices.Add(ind);
                            }
                            stack_free(alloc->stack, (void **) &indices);
                        }

                        stack_free(alloc->stack, (void **) &vertices);
                    }
                    stack_free(alloc->stack, (void **) &faces);
                }
                stack_free(alloc->stack, (void **) &line);
            }
            fclose(f);
        }
        return group;
    }

private:

    static inline int *triangulate_stack(TMeshVertex *vertices, unsigned int nVertices) {
        if (nVertices < 3)
            return nullptr;
        if (nVertices == 3) {
            auto indices = (int *) stack_alloc(alloc->stack, 3 * sizeof(int), sizeof(size_t));
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;
            return indices;
        }

        return nullptr;
    }

    static inline TMeshVertex *generateVertices_stack(
            TStringView *faces,
            unsigned int nFaces,
            TArray<Vec3, TAlloc> &positions,
            TArray<Vec3, TAlloc> &normals,
            TArray<Vec2, TAlloc> &coords) {

        auto meshVertices = (TMeshVertex *) stack_alloc(alloc->stack, nFaces * sizeof(TMeshVertex), sizeof(size_t));
        for (int i = 0; i < nFaces; i++) {
            TStringView *vertices = split_stack(faces[i], '/');
            unsigned int nVertices = stack_n(alloc->stack) / sizeof(TStringView);
            if (nVertices != 3) {
                stack_pop(alloc->stack);
                stack_pop(alloc->stack);
                return nullptr;
            }
            // P/T/N
            TMeshVertex vert;
            vert.Position = element(positions, vertices[0]);
            vert.TexCoord = element(coords, vertices[1]);
            vert.Normal = element(normals, vertices[2]);

            meshVertices[i] = vert;
            stack_pop(alloc->stack);
        }
        return meshVertices;
    }

    template<class T>
    static inline const T &element(TArray<T, TAlloc> &elements, TStringView index) {
        int idx = stoi(index);
        if (idx < 0)
            idx = elements.Length() + idx;
        else
            idx--;
        return elements[idx];
    }

    static inline float stof(TStringView line) {
        char *end = (char *) line.end();
        return strtof(line.begin(), &end);
    }

    static inline int stoi(TStringView line) {
        char *end = (char *) line.end();
        return (int) strtol(line.begin(), &end, 10);
    }

    static inline TStringView firstToken(TStringView line) {
        if (line.empty())
            return "";

        size_t start = line.find_first_not_of(' ');
        size_t end = line.find_first_of(' ', start);

        if (start != TStringView::npos && end != TStringView::npos) {
            return line.substr(start, end - start);
        }

        if (start != TStringView::npos) {
            return line.substr(start);
        }

        return "";
    }

    static inline TStringView lastToken(TStringView line) {
        size_t start = line.find_first_not_of(' ');
        size_t next = line.find_first_of(' ', start);
        start = line.find_first_not_of(' ', next);
        next = line.find_last_not_of(' ');

        if (start != TStringView::npos && next != TStringView::npos) {
            return line.substr(start, next - start + 1);
        }

        if (start != TStringView::npos) {
            return line.substr(start);
        }

        return "";
    }

    static inline TStringView *split_stack(TStringView line, char token) {
        int n = 0;
        int size = sizeof(TStringView);

        auto buffer = (TStringView *) stack_alloc(alloc->stack, size, sizeof(size_t));
        if (line.empty()) {
            stack_expand(alloc->stack, n * size);
            return buffer;
        }

        int prev = 0;
        for (int i = 0; i <= line.length(); i++) {
            if (i == line.length() || line[i] == token) {
                stack_expand(alloc->stack, (n + 1) * size);
                buffer[n] = line.substr(prev, i - prev);
                n++;
                prev = i + 1;
            }
        }

        stack_expand(alloc->stack, n * size);
        return buffer;
    }
};