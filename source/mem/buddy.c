#include "mem/buddy.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"


unsigned int buddy_size(unsigned int order) {
    return sizeof(size_t) + sizeof(BuddyMemory) + (1 << order);
}

BuddyBlock *buddy_next(BuddyBlock *b) {
    return (BuddyBlock *) ((size_t) b + b->size);
}

BuddyMemory *buddy_create(void *m, unsigned int order) {
    const size_t start = (size_t) m;
    const unsigned int padding = MEMORY_PADDING_STD(start);
    const unsigned int space = MEMORY_SPACE_STD(BuddyMemory);

    unsigned int size = 1 << order;
    BuddyMemory *self = (BuddyMemory *) (start + padding);
    self->head = (BuddyBlock *) (start + padding + space);
    self->head->size = size;
    self->head->free = 1;
    self->usage = 0;

    self->tail = buddy_next(self->head);
    self->padding = padding;
    return self;
}

BuddyMemory *make_buddy(unsigned int order) {
    void *m = malloc(buddy_size(order));
    if (m == NULL) {
        printf("buddy: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return buddy_create(m, order);
}

void buddy_destroy(BuddyMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("buddy: destroy failed, invalid instance\n");
        return;
    }
    size_t op = (size_t) (*self) - (*self)->padding;
    free((void *) (op));
    (*self) = NULL;
}

BuddyBlock *buddy_split(BuddyBlock *block, unsigned int size) {
    if (block == NULL || size == 0)
        return NULL;

    while (size < (block->size >> 1)) {
        size_t sz = block->size >> 1;
        block->size = sz;
        block = buddy_next(block);
        block->size = sz;
        block->free = 1;
    }

    if (size > block->size)
        return NULL;

    return block;
}

BuddyBlock *buddy_best(BuddyBlock *head, BuddyBlock *tail, unsigned int size) {
    BuddyBlock *best_block = NULL;
    BuddyBlock *block = head;
    BuddyBlock *buddy = buddy_next(block);

    if (buddy == tail && block->free) return buddy_split(block, size);

    while (block < tail && buddy < tail) {
        if (block->free && buddy->free && block->size == buddy->size) {
            block->size <<= 1;
            if (size <= block->size && (best_block == NULL || block->size <= best_block->size)) best_block = block;
            block = buddy_next(buddy);
            if (block < tail) buddy = buddy_next(block);
            continue;
        }

        if (block->free && size <= block->size && (best_block == NULL || block->size <= best_block->size)) best_block = block;

        if (buddy->free && size <= buddy->size && (best_block == NULL || buddy->size < best_block->size)) best_block = buddy;

        if (block->size <= buddy->size) {
            block = buddy_next(buddy);
            if (block < tail) buddy = buddy_next(block);
        } else {
            block = buddy;
            buddy = buddy_next(buddy);
        }
    }

    if (best_block == NULL) return NULL;
    return buddy_split(best_block, size);
}


void buddy_merge(BuddyBlock *head, BuddyBlock *tail) {
    while (1) {
        BuddyBlock *block = head;
        BuddyBlock *buddy = buddy_next(block);
        char no_merge = 1;
        while (block < tail && buddy < tail) {
            if (block->free && buddy->free && block->size == buddy->size) {
                block->size <<= 1;
                block = buddy_next(block);
                if (block < tail) {
                    buddy = buddy_next(block);
                    no_merge = 0;
                }
            } else if (block->size < buddy->size) {
                block = buddy;
                buddy = buddy_next(buddy);
            } else {
                block = buddy_next(buddy);
                if (block < tail) buddy = buddy_next(block);
            }
        }
        if (no_merge) return;
    }
}

void *buddy_alloc(BuddyMemory *self, unsigned int size) {
    if (self == NULL) {
        printf("buddy: alloc failed, invalid instance\n");
        return NULL;
    }
    if (size == 0) {
        printf("buddy: invalid size\n");
        return NULL;
    }
    const unsigned int space = MEMORY_SPACE_STD(BuddyBlock);
    size += space;

    BuddyBlock *found = buddy_best(self->head, self->tail, size);
    
    if (found == NULL) {
        buddy_merge(self->head, self->tail);
        found = buddy_best(self->head, self->tail, size);
    }

    if (found == NULL) {
        printf("buddy: out of memory!\n");
        return NULL;
    }

    found->free = 0;
    self->usage += found->size;
    return (void *) ((size_t) found + space);
}

char buddy_free(BuddyMemory *self, void **ptr) {
    if (self == NULL) {
        printf("buddy: free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("buddy: free failed, invalid pointer\n");
        return 0;
    }

    size_t head_address = (size_t) (self->head);
    size_t tail_address = (size_t) (self->tail);
    size_t ptr_address = (size_t) (*ptr);

    if (ptr_address < head_address || ptr_address >= tail_address) {
        printf("buddy: free failed, out of boundary\n");
        return 0;
    }

    unsigned int space = MEMORY_SPACE_STD(BuddyBlock);
    BuddyBlock *block = (BuddyBlock *) (ptr_address - space);
    block->free = 1;
    self->usage -= block->size;

    return 1;
}
