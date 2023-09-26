#pragma once

extern "C" {
#include "mesh.h"
}

#include <iostream>
#include "engine/TVector.hpp"
#include "engine/Trace.hpp"

struct TMeshVertex {
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
    CMaterial Material;
    TArray<TMeshVertex, TAlloc> Vertices;
    TArray<int, TAlloc> Indices;

    explicit inline CMesh() = default;

    explicit inline CMesh(const CMesh &) = delete;
};

template<class TAlloc>
struct CMeshGroup {

    TString<TAlloc> Name;
    TFlatMap<TString<TAlloc>, CMesh<TAlloc> *, TAlloc> Meshes;

    explicit inline CMeshGroup() = default;

    explicit inline CMeshGroup(const CMeshGroup &) = delete;

    inline ~CMeshGroup() {
        for (auto p: Meshes) {
            p.first.~TString<TAlloc>();
            Free<TAlloc>(&p.second);
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
                if (token == "#") {
                    stack_pop(alloc->stack);
                    continue;
                }

                if (token == "o") {
                    auto name = lastToken(line);
                    if (group->Meshes.Contains(name))
                        mesh = group->Meshes[name];
                    else {
                        mesh = AllocNew<TAlloc, CMesh<TAlloc>>();
                        mesh->Name = name;
                        group->Meshes.Set(name, mesh);
                    }
                    printf("name: %s \n", name.begin());
                }

                if (token == "v") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 3) positions.Add(vec3(stof(split[0]), stof(split[1]), stof(split[2])));
                    stack_pop(alloc->stack);
                } else if (token == "vn") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 3) normals.Add(vec3(stof(split[0]), stof(split[1]), stof(split[2])));
                    stack_pop(alloc->stack);
                } else if (token == "vt") {
                    TStringView *split = split_stack(lastToken(line), ' ');
                    unsigned int n = stack_n(alloc->stack) / sizeof(TStringView);
                    if (n == 2) coords.Add(vec2(stof(split[0]), stof(split[1])));
                    stack_pop(alloc->stack);
                } else if (token == "f") {
                    TStringView *faces = split_stack(lastToken(line), ' ');
                    unsigned int nFaces = stack_n(alloc->stack) / sizeof(TStringView);
                    TMeshVertex *vertices;
                    if (!(vertices = generateVertices_stack(faces, nFaces, positions, normals, coords))) {
                        stack_pop(alloc->stack);
                        stack_pop(alloc->stack);
                        Free<TAlloc>(&group);
                        return nullptr;
                    }
                    unsigned int nVertices = stack_n(alloc->stack) / sizeof(TMeshVertex);

                    triangulate(vertices, nVertices, mesh);

                    stack_pop(alloc->stack);
                    stack_pop(alloc->stack);
                }
                stack_pop(alloc->stack);
            }
            fclose(f);
        }
        return group;
    }

private:

    static inline bool triangulate(TMeshVertex *vertices, unsigned int nFaces, CMesh<TAlloc> *mesh) {
        if (nFaces < 3)
            return false;
        if (nFaces == 3) {
            for (int i = 0; i < 3; i++) {
                mesh->Vertices.Add(vertices[i]);
                mesh->Indices.Add(i);
            }
            return true;
        }

        return false;
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