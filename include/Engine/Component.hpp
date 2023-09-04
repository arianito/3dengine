#pragma once

extern "C"
{
#include "memory/alloc.h"
}

enum ConfigEnum
{
    CAN_TICK = 1 << 0
};

class Component
{
public:
    ConfigEnum config = CAN_TICK;
    virtual void Create() = 0;
    virtual void Update() = 0;
};

class Entity
{
public:
    ConfigEnum config = CAN_TICK;
    virtual void Create() = 0;
    virtual void Update() = 0;
};

class Director
{
    void AddEntity(const Entity &entity);
};

inline Director *MakeDirector()
{
    Director *director = alloc_global(Director);
    return director;
}