#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_LOG(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif

#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const float RADIUS = 50.0f;
const float SPEED = 150.0f;
const float ASTEROID_SPEED_MIN=80.0f;
const float ASTEROID_SPEED_MAX=180.0f;

#endif

#define STARTING_ASTEROIDS 4
#define MAX_ASTEROIDS 64

typedef enum {
    ASTEROID_LARGE, ASTEROID_MEDIUM, ASTEROID_SMALL
} AsteroidSize;

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    AsteroidSize size;
    bool active;
} Asteroid;

typedef struct {
    Asteroid asteroids[MAX_ASTEROIDS];
    int wave;
} Game;

Vector2 WrapPosition(Vector2 pos) {
    if (pos.x < 0) pos.x += SCREEN_WIDTH;
    if (pos.x > SCREEN_WIDTH) pos.x -= SCREEN_WIDTH;
    if (pos.y < 0) pos.y += SCREEN_HEIGHT;
    if (pos.y > SCREEN_HEIGHT) pos.y -= SCREEN_HEIGHT;

    return pos;
}

void Update(Game *g) {
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &g->asteroids[i];
        if (!a->active) continue;
        a->pos = Vector2Add(a->pos, Vector2Scale(a->velocity, dt));
        a->pos = WrapPosition(a->pos);
        a->rotation = a->rotationSpeed * dt;
    }
}

void Draw(Game *g) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &g->asteroids[i];
        if (!a->active) continue;

        DrawCircleLines(
            (int)a->pos.x, 
            (int)a->pos.y, 
            RADIUS, 
            BLUE
        );
    }
    EndDrawing();
}

static float RandFloat(float min, float max) {
    float t = (float)GetRandomValue(0, 1000000) / 1000000.0f;
    return min + t * (max - min);
}

void SpawnAsteroid(Game *g, Vector2 pos, AsteroidSize size) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!g->asteroids[i].active) {
            float angle = RandFloat(0.0f, 2.0f * PI);
            float speed = RandFloat(ASTEROID_SPEED_MIN, ASTEROID_SPEED_MAX);

            g->asteroids[i] = (Asteroid) {
                .pos = pos,
                .size = size,
                .velocity = (Vector2) { cosf(angle) * speed, sinf(angle) * speed },
                .rotation = RandFloat(0.0f, PI * 2.0f),
                .rotationSpeed = RandFloat(-2.0f, 2.0f),
                .active = true,
            };
            return;
        }
    }
}

void SpawnWave(Game *g) {
    int count = STARTING_ASTEROIDS * (g->wave);
    for (int i = 0; i < count; i++) {
        Vector2 pos;
        if (rand() % 2 == 0) {
            pos.x = (rand() % 2) ? 0.0f : (float) SCREEN_WIDTH;
            pos.y = RandFloat(0.0f, (float) SCREEN_HEIGHT);
        } else {
            pos.x = RandFloat(0.0f, (float) SCREEN_WIDTH);
            pos.y = (rand() % 2) ? 0.0f : (float) SCREEN_HEIGHT;
        }
        SpawnAsteroid(g, pos, ASTEROID_LARGE);
    }
}

void DebugAsteroids(Game *g) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &g->asteroids[i];
        if (!a->active) break;
        DEBUG_LOG(
            "Asteroid %d: pos=(%.2f, %.2f), velocity=(%.2f, %.2f)",
            i,
            a->pos.x,
            a->pos.y,
            a->velocity.x,
            a->velocity.y
        );
    }
}

int main(void) {
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My raylib window");
    SetTargetFPS(60);

    Game g = { 
        .asteroids = { 0 },
        .wave = 1,
    };

    SpawnWave(&g);
    DebugAsteroids(&g);

    while (!WindowShouldClose()) {
        Update(&g);
        Draw(&g);
    }

    CloseWindow();
    return 0;
}