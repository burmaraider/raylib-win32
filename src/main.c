#include "raylib.h"
#include "raymath.h"
#include "GLFW/glfw3.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "win32.h"

float g_nScaleFactor = 1.0f;

#ifdef _MSC_VER
int __stdcall main(void)
#else
int main(void)
#endif
{
    // Initialization
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Raylib Win32 UI");
    SetTargetFPS(60);

    SetupMenubar();
    SetupStatusBar();

    g_nScaleFactor = GetWindowScaleDPI().x; // Get the DPI scale factor

    Texture2D texture = LoadTexture("resources/raylib_logo.png");

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(texture,
                    (GetScreenWidth() / g_nScaleFactor) / 2 - texture.width / 2,
                    (GetScreenHeight() / g_nScaleFactor) / 2 - texture.height / 2,
                    WHITE);
                    
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}