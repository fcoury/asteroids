#include <stdio.h>
#include "raylib.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_LOG(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif

#ifndef CONSTANTS_H
#define CONSTANTS_H

const int WIDTH = 800;
const int HEIGHT = 450;
const float RADIUS = 50.0f;
const float SPEED = 150.0f;

#endif

typedef struct {
    Vector2 pos;
    Vector2 velocity;
} Asteroid;

typedef struct {
    Asteroid *asteroids;
    int asteroidCount;
} Game;

void update(Game *game) {
    float deltaTime = GetFrameTime();
    Asteroid *asteroids = game->asteroids;

    for (int i = 0; i < game->asteroidCount; i++) {
        asteroids[i].pos.x += asteroids[i].velocity.x * deltaTime;
        asteroids[i].pos.y += asteroids[i].velocity.y * deltaTime;

        DEBUG_LOG("Circle %d: x=%.1f y=%.1f", i, asteroids[i].pos.x, asteroids[i].pos.y);

        if (asteroids[i].pos.x - RADIUS > WIDTH) {
            asteroids[i].pos.x = -RADIUS;
        }

        if (asteroids[i].pos.y - RADIUS > WIDTH) {
            asteroids[i].pos.y = -RADIUS;
        }

    }

}

int main(void) {
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(WIDTH, HEIGHT, "My raylib window");
    SetTargetFPS(60);

    Asteroid asteroids[] = {
        { .pos = { 0.0f,  0.0f   }, .velocity = { SPEED,  SPEED }  },
        { .pos = { WIDTH, 0.0f   }, .velocity = { -SPEED, SPEED }  },
        { .pos = { 0.0f,  HEIGHT }, .velocity = { SPEED,  -SPEED } },
        { .pos = { WIDTH, HEIGHT }, .velocity = { -SPEED, -SPEED } },
    };

    const int asteroidCount = sizeof(asteroids) / sizeof(asteroids[0]);

    Game game = { asteroids, asteroidCount };

    float x = 0;
    float y = 0;

    while (!WindowShouldClose()) {
        update(&game);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < asteroidCount; i++) {
            DrawCircleLines(
                (int)asteroids[i].pos.x, 
                (int)asteroids[i].pos.y, 
                RADIUS, 
                BLUE
            );
        }

        DrawText("Hello, raylib!", 300, 215, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}