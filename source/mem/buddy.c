#include "mem/buddy.h"

#include "mem/utils.h"


BuddyBlock *buddy_next(BuddyBlock *b) {
    return (BuddyBlock *) ((size_t) b + b->size);
}

BuddyBlock *buddy_split(BuddyBlock *block, unsigned int size) {
    if (block != NULL && size != 0) {
        while (size < block->size) {
            size_t sz = block->size >> 1;
            block->size = sz;
            block = buddy_next(block);
            block->size = sz;
            block->free = 1;
        }
        if (size <= block->size) {
            return block;
        }
    }
    return NULL;
}

BuddyBlock *buddy_best(BuddyBlock *head, BuddyBlock *tail, unsigned int size) {
    BuddyBlock *best_block = NULL;
    BuddyBlock *block = head;
    BuddyBlock *buddy = buddy_next(block);

    if (buddy == tail && block->free) {
        return buddy_split(block, size);
    }

    while (block < tail && buddy < tail) { // make sure the buddies are within the range
        if (block->free && buddy->free && block->size == buddy->size) {
            block->size <<= 1;
            if (size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
                best_block = block;
            }

            block = buddy_next(buddy);
            if (block < tail) {
                buddy = buddy_next(block);
            }
            continue;
        }


        if (block->free && size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
            best_block = block;
        }

        if (buddy->free && size <= buddy->size && (best_block == NULL || buddy->size < best_block->size)) {
            best_block = buddy;
        }

        if (block->size <= buddy->size) {
            block = buddy_next(buddy);
            if (block < tail)
                buddy = buddy_next(block);
        } else {
            block = buddy;
            buddy = buddy_next(buddy);
        }
    }
    if (best_block != NULL) {
        return buddy_split(best_block, size);
    }
    return NULL;
}

unsigned int buddy_block(BuddyMemory *self, unsigned int size) {
    return 0;
}


unsigned int buddy_size(unsigned int order) {
    return sizeof(size_t) * 2 + sizeof(BuddyMemory) + (1 << order) * 2;
}

BuddyMemory *buddy_create(void *m, unsigned int order) {
    unsigned int size = 1 << order;

    const size_t start = (size_t) m;
    const unsigned int padding = MEMORY_PADDING_STD(start);
    const unsigned int space = MEMORY_SPACE_STD(BuddyMemory);

    BuddyMemory *self = (BuddyMemory *) (start + padding);
    self->head = (BuddyBlock *) (start + padding + space);
    self->head->size = size;
    self->head->free = 1;

    self->tail = buddy_next(self->head);
    self->padding = padding;
    return self;
}