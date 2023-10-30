#pragma once

extern "C" {
#include "mem/freelist.h"
}

#include "engine/mathf.hpp"
#include "data/TArray.hpp"

template<typename T, class TAlloc = FreeListMemory>
class TRTree {
    struct Node {
        BBox boundary;
        TArray<Node *, TAlloc> children{9};

        inline Node(const BBox &bb) : boundary(bb) {}
    };

public:
    Node *root;

    inline TRTree() {
        root = AllocNew<TAlloc, Node>(bbox_empty);
    }
};