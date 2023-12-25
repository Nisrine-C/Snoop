/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
#include "raylib.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Initialization
    InitWindow(screenWidth, screenHeight, "Snoopy's Magic Show");

    // Load title graphic
    Texture2D titleGraphic = LoadTexture("./assets/title.png");

    // Check if the image was loaded successfully
    if (titleGraphic.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load the title image!");
    }

    // Position for the title graphic
    Vector2 titlePosition = { screenWidth / 2 - titleGraphic.width / 2, 100 };

    // Press-to-start prompt
    const char *startPrompt = "Press Enter to Start";
    Vector2 startPromptPosition = { screenWidth / 2 - MeasureText(startPrompt, 20) / 2, 300 };

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw title graphic
        DrawTexture(titleGraphic, titlePosition.x, titlePosition.y, WHITE);

        // Draw press-to-start prompt
        DrawText(startPrompt, startPromptPosition.x, startPromptPosition.y, 20, BLACK);

        EndDrawing();
    }

    // Unload resources
    UnloadTexture(titleGraphic);

    // De-Initialization
    CloseWindow();

    return 0;
}
