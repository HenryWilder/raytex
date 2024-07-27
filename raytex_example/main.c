#include <raylib.h>
#define SUPPORT_TRACELOG
#include <raytex.h>

int main()
{
    InitWindow(1280, 720, "RayTeX Example");
    SetTargetFPS(60);

    RayTeX tex = GenRayTeXText("This sentence \\neq true", TEXMODE_MODE_TEXT, 20, WHITE);

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