#include "alloc.h"
#include "level1.h"

void load()
{
    printf("level-1 loaded\n");
}

void update()
{
    
}

void unload()
{
    printf("level-1 unload\n");
}

Level *level1_new()
{
    Level *level = alloc_global(sizeof(Level));
    level->load = load;
    level->update = update;
    level->unload = unload;
    return level;
}