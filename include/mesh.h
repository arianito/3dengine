#pragma once

#include "mathf.h"
#include "file.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef struct
{
    Vec3 ambient;         // Ka
    Vec3 diffuse;         // Kd
    Vec3 specular;        // Ks
    float specularAmount; // Ns
    float opacity;        // D, Tr
} Material;

typedef struct
{
    Material material;
    Vertex *vertices;
    int *indices;
} Mesh;

inline void mesh_obj(const char *p)
{
    char b[URL_LENGTH];
    resolve(p, b);
    FILE *f;
    fopen_s(&f, b, "r");

    if (f == NULL)
    {
        perror("failed to open the mesh");
        exit(EXIT_FAILURE);
        return;
    }

    char buffer[255];
    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        printf("%s", buffer);
    }

    fclose(f);
}