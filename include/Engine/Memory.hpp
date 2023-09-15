#pragma once

#include "mem/freelist.h"
#include "mem/alloc.h"
#include "engine/Object.hpp"

class Allocator : public Object<Allocator> {
public:
    virtual void *Alloc(size_t size) = 0;

    virtual void Free(void **ptr) = 0;

    virtual void Reset() = 0;
};

class GlobalArenaAllocator : public Allocator {

public:
    inline void *Alloc(size_t size) override {
        return alloc_global(void, size);
    }

    inline void Free(void **ptr) override {
    }

    inline void Reset() override {
    }

    static Allocator *instance() {
        static Allocator *mInstance = nullptr;
        if (mInstance == nullptr)
            mInstance = new GlobalArenaAllocator();
        return mInstance;
    }
};

class GlobalStackAllocator : public Allocator {
public:
    inline void *Alloc(size_t size) override {
        return alloc_stack(void, size);
    }

    inline void Free(void **ptr) override {
        alloc_free(ptr);
    }

    inline void Reset() override {
    }

    static Allocator *instance() {
        static Allocator *mInstance = nullptr;
        if (mInstance == nullptr)
            mInstance = new GlobalStackAllocator();
        return mInstance;
    }
};

class GlobalFreelistAllocator : public Allocator {
public:
    inline void *Alloc(size_t size) override {
        return freelist_alloc(alloc->freelist, size, sizeof(size_t));
    }

    inline void Free(void **ptr) override {
        freelist_free(alloc->freelist, ptr);
    }

    inline void Reset() override {
    }

    static Allocator *instance() {
        static Allocator *mInstance = nullptr;
        if (mInstance == nullptr)
            mInstance = new GlobalFreelistAllocator();
        return mInstance;
    }
};