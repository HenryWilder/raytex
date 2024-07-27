#include <raylib.h>
#define SUPPORT_TRACELOG
#include <raytex.h>

int main()
{
    InitWindow(1280, 720, "RayTeX Example");
    SetTargetFPS(60);

    RayTeX tex = GenRayTeXFraction(
        GenRayTeXText("apple", 20, WHITE),
        GenRayTeXText("orange", 20, WHITE),
        3, WHITE);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRayTeX(tex, 20, 20);

        EndDrawing();
    }

    UnloadRayTeX(tex);

    CloseWindow();
}