#pragma once

typedef struct
{
    void *window;
    float width;
    float height;
    float ratio;
    int fps;
} Game;

typedef struct
{
    float time;
    float deltaTime;
} Time;

Game *game;
Time *time;

void game_init();
char game_loop();
void game_terminate();