#include <raylib.h>
#include <raytex.h>

int main()
{
    InitWindow(1280, 720, "RayTeX Example");
    SetTargetFPS(60);

    RayTeX tex = GenRayTeXText("Hello TeX", TEXMODE_MODE_TEXT, 20, WHITE);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRayTeX(tex, 20, 20, WHITE);

        EndDrawing();
    }

    UnloadRayTeX(tex);

    CloseWindow();
}