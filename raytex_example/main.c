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

    RayTeX tex = GenRayTeXVertical("vvvvv",
        RayTeXColor(GenRayTeXText("Oranges"), ORANGE),
        GenRayTeXSymbol(TEXSYMBOL_NEQ),
        GenRayTeXHorizontal("vvvvv",
            GenRayTeXFrac(
                GenRayTeXText("Pineapple"),
                GenRayTeXText("Strawberry")),
            GenRayTeXSymbol(TEXSYMBOL_NEQ),
            RayTeXColor(GenRayTeXText("Apples"), RED),
            GenRayTeXSymbol(TEXSYMBOL_NEQ),
            GenRayTeXFrac(
                GenRayTeXHorizontal("vv",
                    GenRayTeXText("Apples + "),
                    GenRayTeXFrac(
                        GenRayTeXText("banana"),
                        GenRayTeXText("mango"))),
                GenRayTeXText("Oranges"))),
        GenRayTeXSymbol(TEXSYMBOL_NEQ),
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
