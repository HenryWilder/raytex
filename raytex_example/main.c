#include <raylib.h>
#include <raymath.h>
#include <raytex.h>

Color Rainbow(float t);

int main()
{
    int windowWidth  = 640;
    int windowHeight = 480;
    InitWindow(windowWidth, windowHeight, "RayTeX Example");
    SetTargetFPS(240);

    RayTeX tex = GenRayTeXVertical(VERTICAL_TEXALIGN_CENTER, 3,
        GenRayTeXHorizontal(HORIZONTAL_TEXALIGN_CENTER, 3,
            GenRayTeXText("Apples"),
            GenRayTeXSymbol(TEXSYMBOL_NEQ),
            GenRayTeXFrac(
                GenRayTeXText("Apples"),
                GenRayTeXText("Oranges"),
                4)),
        GenRayTeXSymbol(TEXSYMBOL_NEQ),
        GenRayTeXText("Oranges"));

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRayTeXCentered(tex, 0, 0, windowWidth, windowHeight, 20, BLACK);

        DrawFPS(0,0);
        EndDrawing();
    }

    UnloadRayTeX(tex);
    CloseWindow();
}

Color Rainbow(float t)
{
    return ColorFromHSV(t * 180.0f, 1.0f, 1.0f);
}
