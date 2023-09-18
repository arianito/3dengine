#pragma once


#include "mathf.h"
#include "file.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "mem/alloc.h"

typedef struct {
    Vec3 ambient;          // Ka
    Vec3 diffuse;          // Kd
    Vec3 specular;          // Ks
    float specularAmount; // Ns
    float opacity;          // D, Tr
} Material;

typedef struct {
    Material material;
    Vertex *vertices;
    int *indices;
} Mesh;

static inline void mesh_obj(const char *p) {
    char *path = resolve("%s", p);
    FILE *f;
    fopen_s(&f, path, "r");
    alloc_free((void **) &path);

    if (f == NULL) {
        perror("failed to open the mesh");
        exit(EXIT_FAILURE);
    }

    char buffer[255];
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        printf("%s", buffer);
    }

    fclose(f);
}