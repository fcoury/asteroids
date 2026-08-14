#include <stdio.h>
#include "raylib.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const float radius = 50.0f;
    const float speed = 150.0f;

    InitWindow(screenWidth, screenHeight, "My raylib window");
    SetTargetFPS(60);

    float x = radius;

    while (!WindowShouldClose()) {
        x += speed * GetFrameTime();

        if (x - radius > screenWidth) {
            x = -radius;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleLines((int)x, screenHeight / 2, radius, BLUE);

        DrawText("Hello, raylib!", 300, 215, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}