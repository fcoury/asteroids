#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#define DEBUG 0

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
const float ROT_SPEED = 5.0f;
const float ASTEROID_SPEED_MIN = 80.0f;
const float ASTEROID_SPEED_MAX = 180.0f;
const float SHIP_HEIGHT = 30.0f;
const float SHIP_WIDTH = 15.0f;
const float SHIP_ACCELERATION = 200.0f; // pixels per second squared
const float SHIP_MAX_VELOCITY = 400.0f;
const float DRAG = 0.5f;

#endif

#define STARTING_ASTEROIDS 4
#define MAX_ASTEROIDS 64

typedef enum {
    ASTEROID_LARGE, ASTEROID_MEDIUM, ASTEROID_SMALL
} AsteroidSize;

typedef enum { GAME_OVER, GAME_PLAYING } GameState;

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    float rot;
    float rotSpeed;
    AsteroidSize size;
    bool active;
} Asteroid;

typedef struct {
    Vector2 pos;
    float rot;
    Vector2 velocity;
} Ship;

typedef struct {
    Asteroid asteroids[MAX_ASTEROIDS];
    Ship ship;
    int wave;
    GameState state;
} Game;

Vector2 WrapPosition(Vector2 pos) {
    if (pos.x < 0) pos.x += SCREEN_WIDTH;
    if (pos.x > SCREEN_WIDTH) pos.x -= SCREEN_WIDTH;
    if (pos.y < 0) pos.y += SCREEN_HEIGHT;
    if (pos.y > SCREEN_HEIGHT) pos.y -= SCREEN_HEIGHT;

    return pos;
}

bool ShipCollided(Game *g) {
    float shipRadius = SHIP_HEIGHT / 2.0f;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &g->asteroids[i];
        if (!a->active) continue;

        if (CheckCollisionCircles(
            g->ship.pos,
            shipRadius,
            a->pos,
            RADIUS
        )) {
            return true;
        }
    }

    return false;
}

void Update(Game *g) {
    float dt = GetFrameTime();

    Vector2 forward = Vector2Rotate((Vector2){ 0.0f, -1.0f }, g->ship.rot);

    if (IsKeyDown(KEY_SPACE)) {
        if (g->state == GAME_OVER) {
            g->state = GAME_PLAYING;
        }
    }

    if (IsKeyDown(KEY_LEFT)) {
        g->ship.rot -= ROT_SPEED * dt;
    }

    if (IsKeyDown(KEY_RIGHT)) {
        g->ship.rot += ROT_SPEED * dt;
    }

    if (IsKeyDown(KEY_UP)) {
        Vector2 acceleration = Vector2Scale(forward, SHIP_ACCELERATION);
        g->ship.velocity = Vector2Add(g->ship.velocity, Vector2Scale(acceleration, dt));
    }

    if (IsKeyDown(KEY_DOWN)) {
        Vector2 acceleration = Vector2Scale(forward, -SHIP_ACCELERATION);
        g->ship.velocity = Vector2Add(g->ship.velocity, Vector2Scale(acceleration, dt));
    }

    g->ship.velocity = Vector2Scale(
        g->ship.velocity,
        expf(-DRAG * dt)
    );

    if (Vector2LengthSqr(g->ship.velocity) < 0.01f) {
        g->ship.velocity = (Vector2){ 0.0f, 0.0f };
    }

    g->ship.pos = Vector2Add(
        g->ship.pos,
        Vector2Scale(g->ship.velocity, dt)
    );

    g->ship.pos = WrapPosition(g->ship.pos);

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &g->asteroids[i];
        if (!a->active) continue;
        a->pos = Vector2Add(a->pos, Vector2Scale(a->velocity, dt));
        a->pos = WrapPosition(a->pos);
        a->rot = a->rotSpeed * dt;
    }

    if (ShipCollided(g)) {
        g->state = GAME_OVER;
    }
}

Vector2 Rotate(Vector2 point, Vector2 center, float angle) {
    Vector2 relative = Vector2Subtract(point, center);
    relative = Vector2Rotate(relative, angle);

    return Vector2Add(center, relative);
}

void DrawTail(Game *g) {
    if (!IsKeyDown(KEY_UP)) return;

    float width = 10.0f;
    // float speed = Vector2Length(g->ship.velocity);
    // float speedRatio = fminf(speed / SHIP_MAX_VELOCITY, 1.0f);
    // float height = width * speedRatio;
    float height = 5.0f;

    Rectangle rec = { 
        .x = g->ship.pos.x, 
        .y = g->ship.pos.y, 
        .width = width,
        .height = height,
    };

    Vector2 origin = { width / 2.0f, -SHIP_HEIGHT / 2.0f };
    DrawRectanglePro(rec, origin, g->ship.rot * RAD2DEG, BLACK);
}

void DrawShip(Game *g) {
    // center point above the ship - top of the triangle
    Vector2 px1 = { g->ship.pos.x, g->ship.pos.y - (SHIP_HEIGHT / 2.0f) };
    Vector2 py1 = { g->ship.pos.x - (SHIP_WIDTH / 2.0f), g->ship.pos.y + (SHIP_HEIGHT / 2.0f) };
    Vector2 py2 = { g->ship.pos.x + (SHIP_WIDTH / 2.0f), g->ship.pos.y + (SHIP_HEIGHT / 2.0f) };

    px1 = Rotate(px1, g->ship.pos, g->ship.rot);
    py1 = Rotate(py1, g->ship.pos, g->ship.rot);
    py2 = Rotate(py2, g->ship.pos, g->ship.rot);

    DEBUG_LOG(
        // "Ship: pos=(%.2f, %.2f)", g->ship.pos.x, g->ship.pos.y
        "Ship: velocity=(%.2f, %.2f)", g->ship.velocity.x, g->ship.velocity.y
    );

    DrawLineV(px1, py1, BLACK);
    DrawLineV(py1, py2, BLACK);
    DrawLineV(py2, px1, BLACK);
    DrawTail(g);
}

void DrawTextCenter(const char* text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_WIDTH - textWidth) / 2, y, fontSize, color);
}

void Draw(Game *g) {
    BeginDrawing();
    ClearBackground(RAYWHITE);


    if (g->state == GAME_PLAYING) {
        DrawShip(g);
    } else {
        int fontSize = 30;

        DrawTextCenter(
            "Game Over",
            (SCREEN_HEIGHT - fontSize) / 2,
            fontSize,
            RED
        );
    }

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
                .rot = RandFloat(0.0f, PI * 2.0f),
                .rotSpeed = RandFloat(-2.0f, 2.0f),
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

    Ship s = {
        .pos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        .rot = 0,
        .velocity = 0,
    };

    Game g = { 
        .asteroids = { 0 },
        .wave = 1,
        .ship = s,
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