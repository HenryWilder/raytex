#include <raylib.h>
#include <raymath.h>
#include <raytex.h>

Color Rainbow(float t);

int main()
{
    int windowWidth  = 640;
    int windowHeight = 480;
    InitWindow(windowWidth, windowHeight, "RayTeX Example");
    SetTargetFPS(60);

    RayTeX tex = GenRayTeXVertical("vsvsv",
        RayTeXColor(GenRayTeXText("Oranges"), ORANGE),
        TEXSYMBOL_NEQ,
        GenRayTeXHorizontal("vsvsv",
            GenRayTeXFrac('t','t', "Pineapple", "Strawberry"),
            TEXSYMBOL_NEQ,
            RayTeXColor(GenRayTeXText("Apples"), RED),
            TEXSYMBOL_NEQ,
            GenRayTeXFrac('v','t',
                GenRayTeXHorizontal("vv",
                    GenRayTeXText("Apples + "),
                    GenRayTeXFrac('t','t', "banana", "mango")),
                "Oranges")),
        TEXSYMBOL_NEQ,
        RayTeXColor(GenRayTeXText("Oranges"), ORANGE));

    RayTeX *row2  = RayTeXVerticalChild(&tex, 2);
    RayTeX *frac1 = RayTeXHorizontalChild(row2, 0);
    RayTeX *frac2 = RayTeXHorizontalChild(row2, 4);

    while (!WindowShouldClose())
    {
        Color color = Rainbow((float)GetTime());
        UpdateRayTeXColor(frac1, color);
        UpdateRayTeXColor(frac2, color);

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
    return ColorFromHSV(t * 120.0f, 1.0f, 1.0f);
}
