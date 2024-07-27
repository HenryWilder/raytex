#define SUPPORT_TRACELOG
#include <raylib.h>
#include <raymath.h>
#include <raytex.h>
#include <math.h>

int main()
{
    InitWindow(1280, 720, "RayTeX Example");
    SetTargetFPS(60);

    RayTeX tex = GenRayTeXFraction(
        GenRayTeXFraction(
            GenRayTeXText("apple", 20, RED),
            GenRayTeXText("orange", 20, ORANGE),
            5, WHITE),
        GenRayTeXFraction(
            GenRayTeXText("banana", 20, YELLOW),
            GenRayTeXText("mango", 20, GOLD),
            5, WHITE),
        5, WHITE);


    while (!WindowShouldClose())
    {
        tex.frac[TEXFRAC_DENOMINATOR].frac[TEXFRAC_NUMERATOR].fontSize = (int)Lerp(10.0f, 80.0f, 0.5f * (1.0f + sinf(GetTime())));
        tex.frac[TEXFRAC_NUMERATOR].frac[TEXFRAC_DENOMINATOR].fontSize = (int)Lerp(10.0f, 80.0f, 0.5f * (1.0f + cosf(GetTime())));

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRayTeX(tex, 20, 20);

        EndDrawing();
    }

    UnloadRayTeX(tex);

    CloseWindow();
}