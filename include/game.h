#pragma once

typedef struct
{
    void *window;
    float width;
    float height;
    float ratio;
    float time;
    float delta_time;
    int fps;
} Game;

Game *game;

void game_init();
char game_loop();
void game_terminate();