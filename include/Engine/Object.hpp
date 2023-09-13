#pragma once

#include "mem/alloc.h"

template<class T>
class Object {
public:
    inline void *operator new(size_t size) {
        printf("*%s: %zu\n", typeid(T).name(), size);
        return freelist_alloc(alloc->freelist, size, sizeof(size_t));
    }

    inline void operator delete(void *ptr) {
        if (ptr == NULL)
            return;
        printf("~%s\n", typeid(T).name());
        freelist_free(alloc->freelist, &ptr);
    }
};
