#include <stdio.h>
#include "raylib.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_LOG(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif

typedef struct {
    Vector2 pos;
    Vector2 velocity;
} Asteroid;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const float radius = 50.0f;
    const float speed = 150.0f;

    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenWidth, screenHeight, "My raylib window");
    SetTargetFPS(60);

    Asteroid asteroids[] = {
        { .pos = { 0.0f,        0.0f         }, .velocity = { speed,  speed }  },
        { .pos = { screenWidth, 0.0f,        }, .velocity = { -speed, speed }  },
        { .pos = { 0.0f,        screenHeight }, .velocity = { speed,  -speed } },
        { .pos = { screenWidth, screenHeight }, .velocity = { -speed, -speed } },
    };

    const int asteroidCount = sizeof(asteroids) / sizeof(asteroids[0]);

    float x = 0;
    float y = 0;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        for (int i = 0; i < asteroidCount; i++) {
            asteroids[i].pos.x += asteroids[i].velocity.x * deltaTime;
            asteroids[i].pos.y += asteroids[i].velocity.y * deltaTime;

            DEBUG_LOG("Circle %d: x=%.1f y=%.1f", i, asteroids[i].pos.x, asteroids[i].pos.y);

            if (asteroids[i].pos.x - radius > screenWidth) {
                asteroids[i].pos.x = -radius;
            }
    
            if (asteroids[i].pos.y - radius > screenWidth) {
                asteroids[i].pos.y = -radius;
            }

        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < asteroidCount; i++) {
            DrawCircleLines(
                (int)asteroids[i].pos.x, 
                (int)asteroids[i].pos.y, 
                radius, 
                BLUE
            );
        }

        DrawText("Hello, raylib!", 300, 215, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}