#pragma once

#include <type_traits>
#include <cassert>
#include <cstring>
#include <utility>

extern "C" {
#include "mem/alloc.h"
#include "mem/freelist.h"
#include "mem/stack.h"
#include "mem/arena.h"
}

template<class T, bool Clean = false>
inline void *Alloc(size_t size = -1, unsigned int alignment = sizeof(size_t)) {
    void *m = nullptr;

    if constexpr (std::is_same_v<T, FreeListMemory>) {
        assert(size > 0 && "Alloc<FreeListMemory>: size must be greater than zero. ");
        m = freelist_alloc(alloc->freelist, size, alignment);
    } else if constexpr (std::is_same_v<T, StackMemory>) {
        assert(size > 0 && "Alloc<StackMemory>: size must be greater than zero. ");
        m = stack_alloc(alloc->stack, size, alignment);
    } else if constexpr (std::is_same_v<T, ArenaMemory>) {
        assert(size > 0 && "Alloc<ArenaMemory>: size must be greater than zero. ");
        m = arena_alloc(alloc->global, size, alignment);
    }

    assert(m && "Alloc: allocator class not specified. ");
    if constexpr (Clean) memset(m, 0, size);
    return m;
}

template<class T, typename C, bool Clean = false>
inline C *Alloc(unsigned int length = 1, unsigned int alignment = sizeof(size_t)) {
    return (C *) Alloc<T, Clean>(length * sizeof(C), alignment);
}

template<class T, typename C, typename ...Args>
inline C *AllocNew(Args &&...args) {
    return new(Alloc<T, C>()) C(std::forward<Args>(args)...);
}

template<class T>
inline void Free(void **ptr) {
    if constexpr (std::is_same_v<T, FreeListMemory>) {
        freelist_free(alloc->freelist, ptr);
        return;
    } else if constexpr (std::is_same_v<T, StackMemory>) {
        stack_free(alloc->stack, ptr);
        return;
    } else if constexpr (std::is_same_v<T, ArenaMemory>) {
        return;
    }
    assert(0 && "Free: allocator class not specified. ");
}