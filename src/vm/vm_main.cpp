#include "raylib.h"
#include "rcamera.h"
#include <math.h>
#include <stdio.h>

int main(void)
{

    // Initialization
    //--------------------------------------------------------------------------------------
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
    }

    // De-Initialization

    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}